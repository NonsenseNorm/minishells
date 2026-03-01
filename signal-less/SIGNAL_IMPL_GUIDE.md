# signal 再実装手順書

`signal-less/claude/` に signal 処理を再実装するためのステップバイステップガイドです。  
`claude/` の完成版を参照しながら読むと理解が深まります。

---

## 0. 現状の問題点

signal-less 版では以下の挙動が bash と異なります。

| 操作 | signal-less の挙動 | bash の挙動 |
|------|--------------------|-------------|
| プロンプトで Ctrl+C | プロセス全体が終了する | プロンプトが再表示される |
| プロンプトで Ctrl+\ | `Quit: 3` が出てプロセス終了 | 何も起きない |
| コマンド実行中 Ctrl+C | 子プロセスのみ終了（OK）| 子プロセスのみ終了（OK）|
| コマンド実行中 Ctrl+\ | 子が `Quit: 3` で終了（OK）| 子が `Quit: 3` で終了（OK）|
| heredoc 中 Ctrl+C | プロセス全体が終了する | heredoc をキャンセルしプロンプトに戻る |
| Ctrl+C 直後に Ctrl+D | ループが抜けて exit する | 同様 |

---

## 1. 全体設計の理解

実装すべき「シグナル状態」は 4 種類です。

```
[状態 A] interactive（プロンプト待ち）
  SIGINT  → sig_handler_interactive（readline を即返させ、プロンプト再表示）
  SIGQUIT → SIG_IGN

[状態 B] exec_parent（fork した子を wait 中）
  SIGINT  → SIG_IGN（子が受け取るので親は無視）
  SIGQUIT → SIG_IGN

[状態 C] exec_child（fork 直後の子プロセス）
  SIGINT  → SIG_DFL（親から引き継いだ SIG_IGN を元に戻す）
  SIGQUIT → SIG_DFL

[状態 D] heredoc（heredoc の入力読み取り中）
  SIGINT  → sig_handler_record（g_sig に記録するだけ）
  SIGQUIT → SIG_IGN
```

状態遷移:
```
起動
  └─ A (interactive)
       ├─ コマンド実行開始 → B (exec_parent)
       │    ├─ fork 後の子 → C (exec_child) → execve
       │    └─ wait 完了 → A に戻る
       └─ heredoc 開始 → D (heredoc)
            └─ heredoc 終了 → A に戻る
```

---

## 2. 実装ステップ一覧

| Step | ファイル | 変更内容 |
|------|---------|---------|
| 1 | `src/core/ms.h` | termios インクルード・t_shell フィールド追加・宣言追加 |
| 2 | `src/signal/signal.c` | **新規作成** — 4 関数 + 2 ハンドラ |
| 3 | `src/core/main.c` | `g_sig` 定義・端末制御関数追加・init_shell 更新 |
| 4 | `src/core/loop.c` | `sig_set_interactive` 呼び出し・SIGINT フラッシュ処理 |
| 5 | `src/exec/pipeline.c` | exec 前後のハンドラ切り替え |
| 6 | `src/exec/exec.c` | child_exec でのハンドラリセット |
| 7 | `src/exec/heredoc.c` | heredoc 中のハンドラ切り替えと中断検出 |
| 8 | Makefile | signal.c がビルドに含まれることを確認 |

---

## Step 1: `src/core/ms.h` の更新

### 1-a. `#include <termios.h>` を追加

インクルード群の末尾（`<unistd.h>` の直後）に追加します。

```c
# include <unistd.h>
# include <termios.h>   /* ← 追加 */
```

### 1-b. `t_shell` に端末状態フィールドを追加

```c
typedef struct s_shell
{
    t_env           env;
    t_mem           mem;
    int             exit_code;
    bool            interactive;
    bool            term_saved;    /* ← 追加 */
    struct termios  term_orig;     /* ← 追加 */
}   t_shell;
```

### 1-c. グローバル変数・関数プロトタイプを追加

既存の `void ms_loop(t_shell *sh);` の上あたりに追記します。

```c
extern volatile sig_atomic_t    g_sig;

void    sig_set_interactive(void);
void    sig_set_exec_parent(void);
void    sig_set_exec_child(void);
void    sig_set_heredoc(void);

void    ms_term_disable_echoctl(t_shell *sh);
```

---

## Step 2: `src/signal/signal.c` を新規作成

```c
#include "../core/ms.h"

static void sig_handler_interactive(int sig)
{
    g_sig = sig;
    rl_on_new_line();
    write(STDOUT_FILENO, "\n", 1);
    rl_replace_line("", 0);
    rl_done = 1;   /* readline を即座に "" で返させる */
}

static void sig_handler_record(int sig)
{
    g_sig = sig;   /* 記録だけ。heredoc ループが後で参照する */
}

/*
** Interactive モード:
**   SIGQUIT 無視、SIGINT でプロンプトを再表示。
**   g_sig を 0 にリセットしてから登録する。
*/
void    sig_set_interactive(void)
{
    g_sig = 0;
    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, sig_handler_interactive);
}

/*
** 実行中（親プロセス）:
**   両シグナルを無視。子が同じプロセスグループにいるため
**   端末からのシグナルは子に直接届く。
*/
void    sig_set_exec_parent(void)
{
    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, SIG_IGN);
}

/*
** 実行中（子プロセス）:
**   fork 後に SIG_IGN を引き継いでいるので SIG_DFL に戻す。
**   execve より前に呼ぶこと。
*/
void    sig_set_exec_child(void)
{
    signal(SIGQUIT, SIG_DFL);
    signal(SIGINT, SIG_DFL);
}

/*
** Heredoc モード:
**   SIGQUIT 無視、SIGINT は g_sig に記録（プロセスは死なない）。
**   g_sig を 0 にリセットしてから登録する。
*/
void    sig_set_heredoc(void)
{
    g_sig = 0;
    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, sig_handler_record);
}
```

**注意点:**

- `rl_done = 1` が重要。`rl_redisplay()` に替えると Ctrl+C → Ctrl+D で  
  Ctrl+D を 2 回押さないと exit できなくなるバグが発生します（後述）。
- `sig_handler_interactive` / `sig_handler_record` は `static` にします  
  （外部から直接呼ぶ必要はない）。

---

## Step 3: `src/core/main.c` の更新

### 3-a. グローバル変数の定義

ファイル先頭（インクルードの直後）に追加します。

```c
#include "ms.h"

volatile sig_atomic_t   g_sig = 0;   /* ← 追加 */
```

`extern` 宣言は `ms.h` に、実体の定義はここに 1 か所だけ書きます。

### 3-b. 端末制御関数の追加

```c
/*
** echoctl を無効にすると、Ctrl+C 入力時に端末が "^C" を
** エコーしなくなる。元の設定は sh->term_orig に保存しておく。
*/
void    ms_term_disable_echoctl(t_shell *sh)
{
    struct termios  t;

    if (!isatty(STDIN_FILENO))
        return ;
    if (tcgetattr(STDIN_FILENO, &sh->term_orig) != 0)
        return ;
    sh->term_saved = true;
    t = sh->term_orig;
    t.c_lflag &= ~ECHOCTL;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

static void restore_terminal(t_shell *sh)
{
    if (sh->term_saved)
        tcsetattr(STDIN_FILENO, TCSANOW, &sh->term_orig);
}
```

### 3-c. `init_shell` の更新

```c
static int  init_shell(t_shell *sh, char **envp)
{
    sh->exit_code = 0;
    sh->interactive = isatty(STDIN_FILENO);
    sh->term_saved = false;          /* ← 追加 */
    if (env_init(&sh->env, envp) != 0)
        return (1);
    mem_init(&sh->mem);
    if (!sh->mem.top)
    {
        env_free(&sh->env);
        return (1);
    }
    ms_term_disable_echoctl(sh);     /* ← 追加 */
    return (0);
}
```

### 3-d. `main` の更新

終了前に端末状態を復元します。

```c
int main(int argc, char **argv, char **envp)
{
    t_shell sh;

    (void)argc;
    (void)argv;
    if (init_shell(&sh, envp) != 0)
        return (1);
    ms_loop(&sh);
    clear_history();
    env_free(&sh.env);
    mem_reset(&sh.mem);
    restore_terminal(&sh);           /* ← 追加 */
    return (sh.exit_code);
}
```

---

## Step 4: `src/core/loop.c` の更新

### 4-a. SIGINT フラッシュ関数の追加

```c
/*
** SIGINT 後に readline が "" を返してきたとき、
** exit_code を 130 に設定して次のループに進む。
** g_sig は sig_set_interactive 内でリセット済み。
*/
static void flush_sigint(t_shell *sh)
{
    sh->exit_code = 130;
}
```

### 4-b. `ms_loop` の更新

```c
void    ms_loop(t_shell *sh)
{
    char    *line;

    sig_set_interactive();           /* ← 追加: ループ開始前に登録 */
    while (1)
    {
        line = read_prompt(sh);
        if (!line)
            break ;
        if (g_sig == SIGINT)         /* ← 追加: Ctrl+C 後の空行をフラッシュ */
        {
            flush_sigint(sh);
            free(line);
            continue ;
        }
        if (*line)
            add_history(line);
        else
        {
            free(line);
            continue ;
        }
        handle_line(sh, line);
        free(line);
    }
    if (sh->interactive)
        write(STDOUT_FILENO, "\033[2K\r", 5);
    if (sh->interactive)
        printf("exit\n");
}
```

**`g_sig` チェックの仕組み:**  
`sig_handler_interactive` 内で `rl_done = 1` を設定すると readline は即座に `""` を返します。  
このとき `g_sig == SIGINT` なので `flush_sigint` に分岐し、exit_code=130 を記録して  
`continue` します。`sig_set_interactive` 内で `g_sig = 0` にリセットされるため、  
次のループ先頭では `g_sig == 0` になっています。

---

## Step 5: `src/exec/pipeline.c` の更新

`exec_forked_pipeline` の fork ループ前後にハンドラ切り替えを追加します。

```c
int exec_forked_pipeline(t_shell *sh, t_pipeline *pl)
{
    pid_t   *pids;
    int     p[2];
    int     prev;
    int     i;

    pids = ms_alloc(&sh->mem, sizeof(pid_t) * pl->count);
    prev = -1;
    i = 0;
    sig_set_exec_parent();           /* ← 追加: fork 前に親を SIGINT/QUIT 無視 */
    while (i < pl->count)
    {
        p[0] = -1;
        p[1] = -1;
        if (i < pl->count - 1 && pipe(p) != 0)
            return (1);
        pids[i] = fork();
        if (pids[i] == 0)
            fork_child(sh, &pl->cmds[i], prev, p);
        if (prev != -1)
            close(prev);
        if (p[1] != -1)
            close(p[1]);
        prev = p[0];
        i++;
    }
    if (prev != -1)
        close(prev);
    i = wait_all(pids, pl->count);
    sig_set_interactive();           /* ← 追加: wait 後に interactive に戻す */
    return (i);
}
```

**なぜ `sig_set_exec_parent` を fork ループの外（前）に置くか:**  
fork 後の子が `sig_set_exec_child` で SIG_DFL に戻すので、タイミングに関係なく  
子は正しくシグナルを受け取れます。親は全 fork が終わって wait に入る前から  
SIG_IGN にしておく必要があります。

---

## Step 6: `src/exec/exec.c` の更新

`child_exec` の先頭でシグナルをデフォルトに戻します。

```c
void    child_exec(t_shell *sh, t_cmd *cmd)
{
    char    *path;
    int     ret;

    sig_set_exec_child();            /* ← 追加: 親から引き継いだ SIG_IGN を戻す */
    ret = apply_redirects(sh, cmd->redirects);
    if (ret != 0)
        exit(ret);
    /* ... 以降は変更なし ... */
}
```

**なぜ必要か:**  
`exec_forked_pipeline` では親で `sig_set_exec_parent` (SIG_IGN) を設定してから  
`fork` するため、子プロセスは SIG_IGN を引き継ぎます。`execve` で別コマンドに  
なれば SIG_DFL に戻りますが、`execve` 前（リダイレクト処理中など）にシグナルを  
受け取れるようにするため、ここで明示的にリセットします。

---

## Step 7: `src/exec/heredoc.c` の更新

```c
int heredoc_fd(t_shell *sh, char *delim, bool quoted)
{
    int     p[2];
    char    *line;

    if (pipe(p) != 0)
        return (-1);
    sig_set_heredoc();               /* ← 追加: 記録ハンドラを登録 */
    while (1)
    {
        if (isatty(STDIN_FILENO))
            write(1, "> ", 2);
        line = read_heredoc_line();
        if (!line || g_sig == SIGINT || !ft_strcmp(line, delim))  /* ← g_sig チェック追加 */
            break ;
        heredoc_write(sh, p[1], line, quoted);
        line = NULL;
    }
    if (line)
        free(line);
    close(p[1]);
    sig_set_interactive();           /* ← 追加: interactive に戻す */
    if (g_sig == SIGINT)             /* ← 追加: 中断時は -1 を返す */
        return (close(p[0]), sh->exit_code = 130, -1);
    return (p[0]);
}
```

**`redirect.c` はすでに対応済みです:**  
`apply_redirects` には以下のチェックが既に存在します。

```c
if (fd < 0)
{
    if (r->type == REDIRECT_HEREDOC && sh->exit_code == 130)
        return (130);   /* Ctrl+C による heredoc キャンセル */
    return (ms_perror(r->target, NULL, 1));
}
```

`heredoc_fd` が -1 を返し `sh->exit_code = 130` がセットされている場合、  
`apply_redirects` は 130 を返します。それを受け取った `child_exec` は  
`exit(130)` し、`exec_single_parent` はそのまま 130 を返します。

---

## Step 8: Makefile の確認

Makefile が `find src -name '*.c'` や `**/*.c` 形式で SRCS を取得している場合、  
`src/signal/signal.c` が自動的に含まれます。

手動列挙している場合は `signal/signal.c` を追加してください。

```makefile
# 自動取得の場合（変更不要）
SRCS = $(shell find src -name '*.c')

# 手動列挙の場合（追加が必要）
SRCS = ... \
       src/signal/signal.c \
       ...
```

---

## 9. 動作確認チェックリスト

実装後、以下を bash と比較しながら確認してください。

```sh
# 1. プロンプトで Ctrl+C
#    → "^C" が表示されずに改行+プロンプト再表示
#      （echoctl 実装なしなら "^C" が出るが許容範囲）

# 2. プロンプトで Ctrl+\
#    → 何も起きない（Quit: 3 が出ないこと）

# 3. Ctrl+C 後すぐ Ctrl+D
#    → 1 回の Ctrl+D で "exit" して終了すること

# 4. sleep 10 中に Ctrl+C
#    → sleep が終了し、exit_code が 130 になること
#      echo $? → 130

# 5. sleep 10 中に Ctrl+\
#    → sleep が終了し、"Quit: 3" が表示され exit_code が 131
#      echo $? → 131

# 6. heredoc 中に Ctrl+C
#    → heredoc がキャンセルされプロンプトに戻ること
#      echo $? → 130

# 7. cat | cat 中に Ctrl+C
#    → 両 cat が終了し、プロンプトに戻ること
```

---

## 10. よくある罠と対処法

### 罠 1: `rl_done = 1` ではなく `rl_redisplay()` を使う

`rl_redisplay()` はプロンプトを再描画しますが readline のブロックを解除しません。  
そのため Ctrl+C 後に readline はまだ入力待ちを続け、次の Ctrl+D で NULL が返ります。  
`g_sig == SIGINT` のまま NULL が返るため `flush_sigint` でなく loop 終了と判定され、  
2 回目の Ctrl+D でようやく exit するというバグになります。

**必ず `rl_done = 1` を使うこと。**

### 罠 2: `sig_set_interactive` で `g_sig = 0` のリセットを忘れる

`g_sig` をリセットしないと、次のループで `g_sig == SIGINT` が残り続け、  
すべての空行入力が `flush_sigint` に分岐してしまいます。

### 罠 3: `sig_set_exec_parent` を fork ループの後に置く

子が全部 fork された後に親が SIG_IGN になっても、fork 直後から wait_all までの  
わずかな間に SIGINT を受けると親が終了します。必ず fork ループの前に設定します。

### 罠 4: `child_exec` で `sig_set_exec_child` を呼び忘れる

`execve` は SIG_DFL/SIG_IGN を引き継ぎますが、SIG_DFL は引き継ぎ、SIG_IGN は  
`execve` 後も有効のままです（POSIX 規定）。  
親が SIG_IGN を設定した後に fork した子は SIG_IGN を引き継いでいるため、  
`sig_set_exec_child` で SIG_DFL に戻さないと子コマンドが Ctrl+C で終了しません。

### 罠 5: heredoc の `g_sig` チェックをデリミタ比較の後に置く

```c
/* NG: デリミタ一致でも SIGINT をチェックしてしまう */
if (!line || !ft_strcmp(line, delim) || g_sig == SIGINT)

/* OK: SIGINT を優先して先にチェック */
if (!line || g_sig == SIGINT || !ft_strcmp(line, delim))
```

`sig_handler_record` は `g_sig` に記録するだけで `read` を中断しません。  
read が EINTR で -1 を返すと `read_heredoc_line` は NULL を返すため、  
`!line` でも捕捉できますが、`g_sig == SIGINT` を明示的に並べると意図が明確です。

### 罠 6: `ms_term_disable_echoctl` を interactive でない場合にも呼ぶ

`isatty(STDIN_FILENO)` が false のとき `tcgetattr` はエラーになります。  
`ms_term_disable_echoctl` 内で `isatty` チェックを入れるか、  
`init_shell` 内で `if (sh->interactive)` でガードしてください。

---

## 付録: 変更ファイル一覧

| ファイル | 変更種別 |
|---------|---------|
| `src/core/ms.h` | 変更（インクルード・フィールド・宣言追加） |
| `src/signal/signal.c` | **新規作成** |
| `src/core/main.c` | 変更（g_sig 定義・端末制御追加） |
| `src/core/loop.c` | 変更（ハンドラ呼び出し・SIGINT フラッシュ） |
| `src/exec/pipeline.c` | 変更（exec 前後のハンドラ切り替え） |
| `src/exec/exec.c` | 変更（child_exec 先頭に sig_set_exec_child） |
| `src/exec/heredoc.c` | 変更（ハンドラ切り替え・中断検出） |
| `src/exec/redirect.c` | **変更不要**（exit_code==130 チェック済み） |
