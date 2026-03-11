# 構文解析器 (Parser) 実装ガイド

## パーサーとは何か

レキサーはコマンド文字列をフラットなトークン列に変換した。

```
echo hello | cat > out
↓
[WORD "echo"] → [WORD "hello"] → [PIPE] → [WORD "cat"] → [REDIR_OUT] → [WORD "out"]
```

パーサーはこのトークン列を読んで、「どのコマンドがどの引数・リダイレクトを持つか」
という構造 (`t_pipeline`) に変換する。

```
t_pipeline {
  count = 2
  cmds[0]: argv=["echo","hello"]  redirects=NULL
  cmds[1]: argv=["cat"]           redirects=[{OUT, "out"}]
}
```

---

## データ構造

```c
typedef struct s_pipeline {
    t_cmd   *cmds;   /* コマンドの配列 (count 個) */
    int      count;
} t_pipeline;

typedef struct s_cmd {
    char       **argv;      /* NULL 終端の引数配列 */
    t_redirect  *redirects; /* リダイレクトのリスト */
} t_cmd;

typedef struct s_redirect {
    t_redirect_type  type;   /* REDIRECT_IN/OUT/APPEND/HEREDOC */
    char            *target; /* ファイル名 or ヒアドック区切り文字 */
    struct s_redirect *next;
} t_redirect;
```

パーサーは `malloc` で各構造体を確保する。
`free_pipeline` がすべてを解放する (実装済み)。

---

## 関数の呼び出し構造

```
parse_pipeline(tok, &pl)
 │
 ├─ count_cmds(tok)       … | の数 + 1 でコマンド数を求める
 ├─ malloc(pl->cmds)      … cmds 配列を確保
 └─ parse_cmds(tok, &pl)  … 各コマンドを順に解析
      │
      └─ parse_one(&tok, &cmd[i]) × count 回
           │
           ├─ 第1パス: argc をカウントして argv を malloc
           └─ 第2パス: argv と redirects を埋める
                └─ add_redirect(cmd, tok) … リダイレクト追加
```

`parse_cmds` と `add_redirect` はすでに実装済み。
実装が必要なのは **`count_cmds`** と **`parse_one`** の2関数のみ。

---

## Step 1 — `count_cmds`: パイプ数を数える

**ゴール**: トークンリストを走査し、`TOK_PIPE` の数 + 1 を返す。

```
[WORD][PIPE][WORD]              → 2
[WORD][PIPE][WORD][PIPE][WORD]  → 3
[WORD][REDIR_OUT][WORD]         → 1  (パイプなし)
```

実装方針:

```c
static int count_cmds(t_token *tok)
{
    int count;

    count = 1;
    while (tok)
    {
        if (tok->type == TOK_PIPE)
            count++;
        tok = tok->next;
    }
    return (count);
}
```

**確認**: `make && echo 'echo | cat' | ./parser` を実行。
トークン列は表示されるが、まだパイプライン構造は出ない
(Step 2 / 3 が未実装のため)。

---

## Step 2 — `parse_pipeline`: シンタックスチェックを追加する

**ゴール**: 先頭が `NULL` (空行) または `TOK_PIPE` のときエラーを返す。

`parser.c` の `parse_pipeline` にある `/* TODO: Step 2 */` の行を
以下に差し替える:

```c
if (!tok || tok->type == TOK_PIPE)
    return (syntax_err(tok));
```

`syntax_err` はすでに実装済みで、bash と同じメッセージを出力する:

```
minishell: syntax error near unexpected token `|'
minishell: syntax error near unexpected token `newline'
```

**確認**:
```
parse> |
minishell: syntax error near unexpected token `|'
parse> (空Enter)
(エラーなし — 空行は main.c で弾かれる)
```

---

## Step 3 — `parse_one`: 1コマンドの解析 (核心部分)

これがパーサーの本体。2パスで実装する。

### parse_one の入出力

```
入力:  *tokp → [WORD "echo"][WORD "hello"][PIPE]...
出力:  cmd->argv = {"echo", "hello", NULL}
       cmd->redirects = NULL
       *tokp → [PIPE]...    ← 次のパイプの直前まで進める
```

リダイレクトがある場合:

```
入力:  *tokp → [WORD "cat"][REDIR_OUT][WORD "out"][PIPE]...
出力:  cmd->argv = {"cat", NULL}
       cmd->redirects = [{OUT, "out"}]
       *tokp → [PIPE]...
```

### リダイレクトトークンの扱い

リダイレクトはトークンが **2個ペア** になっている:

```
[REDIR_OUT]  [WORD "out"]
  ↑ tok         ↑ tok->next (ファイル名)
```

`is_redirect(tok->type)` が真なら、次のトークンがターゲットになる。

### 第1パス: argc を数える

```c
/* argc を数えて argv を確保する */
tok = *tokp;
argc = 0;
while (tok && tok->type != TOK_PIPE)
{
    if (tok->type == TOK_WORD)
        argc++;
    else if (is_redirect(tok->type) && tok->next)
        tok = tok->next; /* リダイレクトターゲットをスキップ */
    tok = tok->next;
}
cmd->argv = malloc(sizeof(char *) * (argc + 1));
if (!cmd->argv)
    return (1);
/* argv を NULL で初期化 */
k = 0;
while (k <= argc)
    cmd->argv[k++] = NULL;
cmd->redirects = NULL;
```

**注意**: `argc + 1` 個確保して最後を `NULL` にする (execve の規約)。

### 第2パス: argv と redirects を埋める

```c
tok = *tokp;
k = 0;
while (tok && tok->type != TOK_PIPE)
{
    if (tok->type == TOK_WORD)
        cmd->argv[k++] = tok->value;
    else
    {
        /* リダイレクト: 次とーくんが存在しかつ WORD でなければエラー */
        if (!tok->next || tok->next->type != TOK_WORD)
            return (syntax_err(tok->next));
        if (add_redirect(cmd, tok) != 0)
            return (1);
        tok = tok->next; /* ターゲットをスキップ */
    }
    tok = tok->next;
}
*tokp = tok; /* PIPE または NULL を指す */
return (0);
```

### 完成形のイメージ

```c
static int parse_one(t_token **tokp, t_cmd *cmd)
{
    t_token *tok;
    int      argc;
    int      k;

    /* --- 第1パス: argc をカウント --- */
    tok = *tokp;
    argc = 0;
    while (tok && tok->type != TOK_PIPE)
    {
        if (tok->type == TOK_WORD)
            argc++;
        else if (is_redirect(tok->type) && tok->next)
            tok = tok->next;
        tok = tok->next;
    }

    /* --- argv を確保して NULL 初期化 --- */
    cmd->argv = malloc(sizeof(char *) * ((size_t)argc + 1));
    if (!cmd->argv)
        return (1);
    k = 0;
    while (k <= argc)
        cmd->argv[k++] = NULL;
    cmd->redirects = NULL;

    /* --- 第2パス: argv と redirects を埋める --- */
    tok = *tokp;
    k = 0;
    while (tok && tok->type != TOK_PIPE)
    {
        if (tok->type == TOK_WORD)
            cmd->argv[k++] = tok->value;
        else
        {
            if (!tok->next || tok->next->type != TOK_WORD)
                return (syntax_err(tok->next));
            if (add_redirect(cmd, tok) != 0)
                return (1);
            tok = tok->next;
        }
        tok = tok->next;
    }
    *tokp = tok;
    return (0);
}
```

また、Step 3 で実装する前に `parser.c` の以下の行を削除する:

```c
(void)add_redirect; /* Step 3 で使う — この行は消してよい */
```

### 動作確認

```
parse> echo hello | cat > out
=== tokens ===
[WORD "echo"] -> [WORD "hello"] -> [PIPE] -> [WORD "cat"] -> [REDIR_OUT] -> [WORD "out"]
=== pipeline: 2 cmd(s) ===
  cmd[0] argv: "echo" "hello"
  cmd[1] argv: "cat"
  cmd[1] redir: > "out"
```

---

## シンタックスエラーの一覧

| 入力例           | エラー箇所        | 出力                                             |
|------------------|-------------------|--------------------------------------------------|
| `\| echo`        | 先頭が `\|`       | `syntax error near unexpected token '\|'`        |
| `echo \|`        | `\|` の後が NULL  | `syntax error near unexpected token 'newline'`   |
| `echo \| \| cat` | 2つ目の `\|`      | `syntax error near unexpected token '\|'`        |
| `echo >`         | `>` の後が NULL   | `syntax error near unexpected token 'newline'`   |

---

## デバッグのヒント

### トークンを printf で確認する

`parse_one` の内部で迷ったら tok を直接 printf で確認する:

```c
printf("tok: type=%d value=%s\n",
    tok->type, tok->value ? tok->value : "(nil)");
```

### ステップごとに実行して確認する

```bash
make && echo 'echo hello' | ./parser          # 単純コマンド
make && echo 'echo | cat' | ./parser          # パイプ
make && echo 'cat < in > out' | ./parser      # リダイレクト
make && echo '| echo' | ./parser              # シンタックスエラー
make && echo "echo '|'" | ./parser            # クォート内のパイプ
```

---

## まとめ: 実装チェックリスト

- [ ] **Step 1**: `count_cmds` — `TOK_PIPE` の数 + 1 を返す
- [ ] **Step 2**: `parse_pipeline` — 先頭 `NULL`/`TOK_PIPE` でエラー
- [ ] **Step 3**: `parse_one` — 第1パス (argc カウント + malloc)
- [ ] **Step 3**: `parse_one` — 第2パス (argv と redirects を埋める)
- [ ] **確認**: `add_redirect` の `(void)` 行を削除

`parse_cmds` はスキャフォールド済みなので変更不要。
