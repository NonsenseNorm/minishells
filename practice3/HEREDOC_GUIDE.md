# ヒアドキュメント (Heredoc) 実装ガイド

## ヒアドックとは何か

`<<` はヒアドキュメントを意味するリダイレクト演算子だ。
区切り文字 (delimiter) を入力するまでの複数行テキストを stdin として渡せる。

```bash
cat << EOF
hello
world
EOF
```

上記は `echo "hello\nworld" | cat` と同じ動作をする。

---

## このプラクティスの前提

practice2 の exec.c が完成済みとして提供されている。
`exec_pipeline` は `prepare_pipeline_heredocs(pl)` を呼び、
その後フォーク・パイプ処理を行う。

**あなたが実装するのは `heredoc.c` の3関数のみ。**

---

## ヒアドックの処理フロー

```
exec_pipeline(pl)
 │
 ├─ prepare_pipeline_heredocs(pl)  ← Step 3 で実装
 │    │
 │    └─ write_heredoc_to_pipe(r)  ← Step 2 で実装  (heredoc リダイレクトごとに呼ぶ)
 │         │
 │         ├─ read_heredoc(delimiter)  ← Step 1 で実装
 │         │    readline ループで区切り文字まで読み込む
 │         │
 │         ├─ pipe(pipe_fd)
 │         ├─ write(pipe_fd[1], content, ...)
 │         ├─ close(pipe_fd[1])
 │         └─ r->hd_fd = pipe_fd[0]  ← exec.c が後で STDIN に dup2
 │
 └─ [通常の fork/pipe/exec 処理へ]
       子プロセスで apply_redirections が r->hd_fd を STDIN_FILENO に dup2
```

---

## `t_redirect` の `hd_fd` フィールド

`types.h` の `t_redirect` に `int hd_fd` が追加されている:

```c
typedef struct s_redirect {
    t_redirect_type  type;
    char            *target;  /* ファイル名 または heredoc の区切り文字 */
    int              hd_fd;   /* heredoc のパイプ読み端 (-1 で初期化済み) */
    struct s_redirect *next;
} t_redirect;
```

`write_heredoc_to_pipe` がパイプを作り、読み端を `r->hd_fd` に保存する。
exec.c の `apply_redirections` がこの値を使って `dup2(r->hd_fd, STDIN_FILENO)` する。

---

## Step 1 — `read_heredoc`: readline ループで読み込む

**ゴール**: `delimiter` が入力されるまで行を読み続け、コンテンツを返す。

```
入力:
> hello
> world
> EOF    ← delimiter と一致

戻り値: "hello\nworld\n"
```

実装方針:

```c
char *read_heredoc(const char *delimiter)
{
    char *line;
    char *content;
    char *tmp;

    content = strdup("");
    while (1)
    {
        line = readline("> ");
        if (!line)
            break;               /* EOF (Ctrl-D) で中断 */
        if (strcmp(line, delimiter) == 0)
        {
            free(line);
            break;               /* 区切り文字と一致 → 終了 */
        }
        /* content に line + "\n" を追記する */
        tmp = malloc(strlen(content) + strlen(line) + 2);
        /* ... strcpy / strcat で組み立て ... */
        free(content);
        content = tmp;
        free(line);
    }
    return (content);
}
```

**確認**: `read_heredoc("EOF")` を直接呼んで printf で出力してみる。

---

## Step 2 — `write_heredoc_to_pipe`: パイプにコンテンツを書き込む

**ゴール**: ヒアドックのコンテンツをパイプに書き込み、読み端を `r->hd_fd` に保存する。

```
            write(pipe_fd[1], content)
[parent] ─────────────────────────────▶ pipe[1] (close)
                                            │
                                        pipe[0] ──▶ [child stdin]
                                         ↑
                                    r->hd_fd に保存
```

実装方針:

```c
static int write_heredoc_to_pipe(t_redirect *r)
{
    char *content;
    int   pipe_fd[2];

    content = read_heredoc(r->target);
    if (!content)
        return (1);
    if (pipe(pipe_fd) == -1)
    {
        free(content);
        return (1);
    }
    write(pipe_fd[1], content, strlen(content));
    close(pipe_fd[1]);   /* 重要: 閉じないと子が EOF を受け取れない */
    r->hd_fd = pipe_fd[0];
    free(content);
    return (0);
}
```

---

## Step 3 — `prepare_pipeline_heredocs`: パイプライン全体を処理する

**ゴール**: パイプライン内のすべての `REDIRECT_HEREDOC` に `write_heredoc_to_pipe` を呼ぶ。

```c
int prepare_pipeline_heredocs(t_pipeline *pl)
{
    int         i;
    t_redirect *r;

    i = 0;
    while (i < pl->count)
    {
        r = pl->cmds[i].redirects;
        while (r)
        {
            if (r->type == REDIRECT_HEREDOC)
            {
                if (write_heredoc_to_pipe(r) != 0)
                    return (1);
            }
            r = r->next;
        }
        i++;
    }
    return (0);
}
```

---

## Step 4 — シグナルハンドリング (SIGINT 中断)

**ゴール**: ヒアドック入力中に Ctrl-C を押すと読み込みを中断する。

`heredoc.c` の先頭にある `g_heredoc_interrupted` フラグを使う。

### シグナルハンドラの追加

```c
static void heredoc_sigint_handler(int sig)
{
    (void)sig;
    g_heredoc_interrupted = 1;
    /* readline に割り込みを通知する */
    rl_replace_line("", 0);
    rl_done = 1;
}
```

### read_heredoc 内での設定

```c
char *read_heredoc(const char *delimiter)
{
    /* ループ前にシグナルハンドラを設定 */
    g_heredoc_interrupted = 0;
    signal(SIGINT, heredoc_sigint_handler);
    signal(SIGQUIT, SIG_IGN);
    rl_event_hook = check_heredoc_interrupted; /* optional */

    /* ... readline ループ ... */

    /* ループ後にシグナルハンドラを元に戻す */
    signal(SIGINT, SIG_DFL);
    return (content);
}
```

SIGINT で中断した場合は `content` を `free` して `NULL` を返すことで、
`prepare_pipeline_heredocs` が 1 を返しパイプライン実行をスキップできる。

---

## 動作確認

```bash
make

# Step 1/2/3 確認
exec> cat << EOF
> hello
> world
> EOF
hello
world
[exit: 0]

# パイプとの組み合わせ
exec> cat << EOF | tr a-z A-Z
> hello world
> EOF
HELLO WORLD
[exit: 0]

# 複数ヒアドック (Step 3 が正しく複数を処理できるか)
exec> paste <(cat << A) <(cat << B)   ← bash でのテスト例

# Step 4 確認: ヒアドック入力中に Ctrl-C
exec> cat << EOF
> (ここで Ctrl-C)
[exit: 1 または シグナル終了コード]
```

---

## よくあるミス

### close し忘れ

```c
write(pipe_fd[1], content, strlen(content));
/* close(pipe_fd[1]); を忘れると子プロセスが EOF を待ち続けてハングする */
```

### `\n` の追記忘れ

```c
/* readline は行末の \n を含まない */
/* "hello\n" ではなく "hello" が返ってくる */
/* 自分で "\n" を追加しないと行が詰まってしまう */
```

### hd_fd を閉じ忘れ

子プロセスで `dup2(r->hd_fd, STDIN_FILENO)` した後、必ず `close(r->hd_fd)` すること。
`exec.c` の `apply_redirections` に実装済み。

---

## まとめ: 実装チェックリスト

- [ ] **Step 1**: `read_heredoc` — readline ループで delimiter まで読み込み
- [ ] **Step 1**: `read_heredoc` — `content` に `line + "\n"` を追記
- [ ] **Step 2**: `write_heredoc_to_pipe` — pipe 作成 → write → close → hd_fd 設定
- [ ] **Step 3**: `prepare_pipeline_heredocs` — 全コマンドの heredoc を処理
- [ ] **Step 4**: (発展) SIGINT ハンドラで `g_heredoc_interrupted` をセット
- [ ] **確認**: `cat << EOF` で複数行入力 → 正しく出力される
- [ ] **確認**: パイプとの組み合わせ `cat << EOF | wc -l` が動く
