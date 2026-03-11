# 実行エンジン (Exec) 実装ガイド

## exec とは何か

パーサーはコマンド文字列を `t_pipeline` 構造体に変換した。

```
t_pipeline {
  count = 2
  cmds[0]: argv=["echo","hello"]  redirects=NULL
  cmds[1]: argv=["cat"]           redirects=[{OUT, "out"}]
}
```

exec はこの構造体を受け取り、実際にプロセスを起動・パイプ接続・リダイレクトを行う。

---

## 処理の全体像

```
exec_pipeline(pl)
 │
 ├─ pipe() でコマンド間のパイプを作る
 ├─ exec_one(cmd, pipe_in, pipe_out) × count 回
 │    │
 │    ├─ fork()
 │    │    │
 │    │    ├─ [子] setup_child_io    … dup2 でパイプを stdin/stdout に繋ぐ
 │    │    ├─ [子] apply_redirections … ファイルリダイレクトを適用
 │    │    ├─ [子] find_cmd          … $PATH からコマンドを探す
 │    │    └─ [子] execve            … プロセスを置き換える
 │    │
 │    └─ [親] pid を返す
 │
 └─ waitpid() で全子プロセスの終了を待つ
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
```

---

## Step 1 — `find_cmd`: コマンドのフルパスを探す

**ゴール**: argv[0] からコマンドの実行可能ファイルのフルパスを返す。

```
argv[0] = "ls"    → "/usr/bin/ls"  ($PATH を検索)
argv[0] = "./a.out" → "./a.out"  (スラッシュ含む → そのまま)
argv[0] = "nosuchcmd" → NULL     (見つからない)
```

実装方針:

```c
char *find_cmd(const char *argv0)
{
    /* スラッシュが含まれる → パスが明示されている */
    if (strchr(argv0, '/'))
        return (strdup(argv0));

    /* $PATH を取得して split_path で分割 */
    path_env = getenv("PATH");
    dirs = split_path(path_env);

    /* 各ディレクトリで "dir/argv0" を試す */
    while (dirs[i] && !candidate)
    {
        len = strlen(dirs[i]) + 1 + strlen(argv0) + 1;
        candidate = malloc(len);
        snprintf(candidate, len, "%s/%s", dirs[i], argv0);
        if (access(candidate, X_OK) != 0)
        {
            free(candidate);
            candidate = NULL;
        }
        i++;
    }
    free_arr(dirs);
    return (candidate);  /* NULL if not found */
}
```

**確認**: `make && echo 'ls' | ./minishell` を実行してみる。
（Step 4 / 5 が未実装なので実行はまだできないが、find_cmd 単体で printf デバッグ可能）

---

## Step 2 — `setup_child_io`: パイプを stdin/stdout に繋ぐ

**ゴール**: `dup2` で pipe fd を標準入出力に差し替える。

```
prev_cmd  ──pipe_in──▶  [この子プロセス]  ──pipe_out──▶  next_cmd
                stdin                       stdout
```

```c
static void setup_child_io(int pipe_in, int pipe_out)
{
    if (pipe_in >= 0)
    {
        dup2(pipe_in, STDIN_FILENO);
        close(pipe_in);
    }
    if (pipe_out >= 0)
    {
        dup2(pipe_out, STDOUT_FILENO);
        close(pipe_out);
    }
}
```

**注意**: `dup2` の後に元の fd を必ず `close` すること。
しないとパイプが閉じられず後続コマンドが EOF を受け取れなくなる。

---

## Step 3 — `apply_redirections`: ファイルリダイレクト

**ゴール**: `t_redirect` リストを走査してファイルを開き fd を繋ぐ。

| type             | open フラグ                    | dup2 先        |
|------------------|-------------------------------|----------------|
| REDIRECT_IN      | `O_RDONLY`                    | STDIN_FILENO   |
| REDIRECT_OUT     | `O_WRONLY\|O_CREAT\|O_TRUNC`  | STDOUT_FILENO  |
| REDIRECT_APPEND  | `O_WRONLY\|O_CREAT\|O_APPEND` | STDOUT_FILENO  |

```c
static int apply_redirections(t_redirect *r)
{
    int fd;

    while (r)
    {
        if (r->type == REDIRECT_IN)
            fd = open(r->target, O_RDONLY);
        else if (r->type == REDIRECT_OUT)
            fd = open(r->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        else if (r->type == REDIRECT_APPEND)
            fd = open(r->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
        else { r = r->next; continue; }

        if (fd == -1) { perror(r->target); return (1); }
        dup2(fd, (r->type == REDIRECT_IN) ? STDIN_FILENO : STDOUT_FILENO);
        close(fd);
        r = r->next;
    }
    return (0);
}
```

---

## Step 4 — `exec_one`: 1コマンドを子プロセスで実行

**ゴール**: `fork()` → 子で setup/exec、親で pid を返す。

```c
static pid_t exec_one(t_cmd *cmd, int pipe_in, int pipe_out)
{
    pid_t  pid;
    char  *path;

    pid = fork();
    if (pid < 0) { perror("fork"); return (-1); }
    if (pid == 0)
    {
        /* 子プロセス */
        setup_child_io(pipe_in, pipe_out);
        if (apply_redirections(cmd->redirects) != 0)
            exit(1);
        if (!cmd->argv || !cmd->argv[0])
            exit(0);
        path = find_cmd(cmd->argv[0]);
        if (!path)
        {
            fprintf(stderr, "minishell: %s: command not found\n",
                cmd->argv[0]);
            exit(127);
        }
        execve(path, cmd->argv, environ);
        perror(path);
        free(path);
        exit(126);
    }
    /* 親プロセス */
    return (pid);
}
```

**注意**:
- `fork()` 後に子プロセスは `exit()` で終了し、`return` しないこと
- `execve` の第3引数には `extern char **environ` を使う

---

## Step 5 — `exec_pipeline`: パイプラインを実行

**ゴール**: コマンドをパイプで繋いで実行し、最後のコマンドの終了コードを返す。

パイプの接続イメージ:

```
cmd[0]  ──pipe[0]──▶  cmd[1]  ──pipe[1]──▶  cmd[2]
(pipe_in=-1)          (pipe_in=pipe[0][0])   ...
(pipe_out=pipe[0][1]) (pipe_out=pipe[1][1])  (pipe_out=-1)
```

```c
int exec_pipeline(t_pipeline *pl)
{
    int    i;
    int    prev_fd;       /* 前のパイプの読み端 */
    int    pipe_fd[2];
    pid_t *pids;
    int    status;
    int    last_status;

    pids = malloc(sizeof(pid_t) * (size_t)pl->count);
    prev_fd = -1;
    i = 0;
    while (i < pl->count)
    {
        /* 最後のコマンド以外はパイプを作る */
        if (i < pl->count - 1)
            pipe(pipe_fd);
        else
            pipe_fd[0] = pipe_fd[1] = -1;

        pids[i] = exec_one(&pl->cmds[i], prev_fd, pipe_fd[1]);

        /* 親: 不要な fd を閉じる */
        if (prev_fd >= 0) close(prev_fd);
        if (pipe_fd[1] >= 0) close(pipe_fd[1]);
        prev_fd = pipe_fd[0];   /* 次のループで pipe_in になる */
        i++;
    }
    if (prev_fd >= 0) close(prev_fd);

    /* 全子プロセスを wait */
    last_status = 0;
    i = 0;
    while (i < pl->count)
    {
        if (pids[i] > 0)
        {
            waitpid(pids[i], &status, 0);
            last_status = WEXITSTATUS(status);
        }
        i++;
    }
    free(pids);
    return (last_status);
}
```

**重要**: 親プロセスで `pipe_fd[1]` を閉じないと、次コマンドが stdin の EOF を受け取れず無限に待ち続ける。

---

## 動作確認

```bash
make

# Step 1 確認 (コマンド検索)
exec> ls

# Step 4/5 確認 (パイプ)
exec> echo hello | cat
exec> ls | grep .c | wc -l

# Step 3 確認 (リダイレクト)
exec> echo hello > /tmp/out
exec> cat < /tmp/out
exec> echo world >> /tmp/out
exec> cat /tmp/out
```

期待される出力:
```
exec> ls
[exit: 0 が表示されてから ls の結果]
exec> echo hello | cat
hello
[exit: 0]
```

---

## デバッグのヒント

### fork/exec の動きを確認する

```c
/* exec_one の fork 後に追加 */
if (pid == 0)
    fprintf(stderr, "[child pid=%d] cmd=%s\n", getpid(), cmd->argv[0]);
else
    fprintf(stderr, "[parent] launched pid=%d\n", pid);
```

### パイプの fd リークを確認する

```c
/* exec_pipeline のループ内で */
fprintf(stderr, "prev_fd=%d pipe_fd=[%d,%d]\n",
    prev_fd, pipe_fd[0], pipe_fd[1]);
```

---

## まとめ: 実装チェックリスト

- [ ] **Step 1**: `find_cmd` — スラッシュなし → $PATH 検索、スラッシュあり → strdup
- [ ] **Step 2**: `setup_child_io` — pipe_in/pipe_out を dup2 → close
- [ ] **Step 3**: `apply_redirections` — 各リダイレクトタイプでファイルを開いて dup2
- [ ] **Step 4**: `exec_one` — fork → 子: setup/redir/exec、親: pid 返却
- [ ] **Step 5**: `exec_pipeline` — pipe 作成 → exec_one ループ → wait
