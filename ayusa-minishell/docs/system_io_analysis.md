# minishell システム I/O 分析

---

## 入力

| # | 入力種別 | ソース | 経路 |
|---|----------|--------|------|
| 1 | TTY インタラクティブ入力 | 端末 (TTY) | `readline(PROMPT)` → `exec_multiline()` → `exec_line()` → lexer → parser → executor |
| 2 | ヒアドキュメント入力 | 端末 (TTY) | `readline("ウラ> ")` (heredoc 子プロセス内) → 展開 → `/tmp/.minishell_heredoc_*` に書き込み → 実行時 `dup2(fd, STDIN_FILENO)` |
| 3 | ファイルリダイレクト入力 | ファイルシステム | `open(filename, O_RDONLY)` → `dup2(fd, STDIN_FILENO)` → コマンドの stdin |
| 4 | パイプ入力 | 前コマンドの stdout | `pipe(fd)` → 右子プロセス: `dup2(fd[0], STDIN_FILENO)` → コマンドの stdin |
| 5 | 環境変数 (起動時) | `main()` の `envp` 引数 | `env_init(envp)` → `"KEY=VALUE"` をパースして `t_env` 連結リストに格納 |
| 6 | シェル変数展開 (`$VAR`, `$?`) | `t_env` 連結リスト | `expand_string()` → `handle_dollar()` → `get_env_value()` → 展開済み文字列 |
| 7 | ワイルドカード展開 (`*`) | ファイルシステム | `expand_wildcard()` → `opendir()` + `readdir()` → `match_pattern()` → マッチしたファイル名配列 |
| 8 | シグナル入力 (`SIGINT`, `SIGQUIT`) | キーボード (Ctrl+C / Ctrl+\\) | OS カーネルがシグナル配送 → 各ハンドラ or `SIG_DFL` / `SIG_IGN` → `g_sig` グローバル変数に記録 |
| 9 | `/dev/urandom` | カーネル乱数デバイス | `open("/dev/urandom", O_RDONLY)` + `read(fd, buf, 4)` → ヒアドキュメント一時ファイル名のサフィックス生成 |
| 10 | ブラケットペースト (複数行) | 端末 (TTY) | `readline()` が `\n` を含む文字列を返す → `exec_multiline()` で `ft_split_c(line, '\n')` → 各行を個別に `exec_line()` |
| 11 | 子プロセスの終了ステータス | カーネル (子プロセス終了) | `waitpid(pid, &status, 0)` → `calculate_exit_status()` → `update_exit_status()` → `$?` に格納 |

---

## 出力

| # | 出力種別 | 宛先 | 生成箇所 |
|---|----------|------|----------|
| 1 | `echo` の引数テキスト | stdout | `builtin_echo` → `ft_putstr_fd(..., STDOUT_FILENO)` |
| 2 | `pwd` のカレントディレクトリ | stdout | `builtin_pwd` → `getcwd()` + `ft_putendl_fd` |
| 3 | `env` の環境変数一覧 (`KEY=VALUE`) | stdout | `builtin_env` |
| 4 | `export` 引数なし時の変数一覧 (`declare -x KEY="VALUE"`) | stdout | `print_exported_env` |
| 5 | `cd -` 実行時の OLDPWD パス | stdout | `cd.c:73` → `ft_putendl_fd(*path, STDOUT_FILENO)` |
| 6 | 外部コマンドの stdout | stdout (または pipe / ファイル) | `execve` 経由で子プロセスが書き込む |
| 7 | メインプロンプト (`"何か打ち込んでみろッ！"`) | 端末 (TTY) | `readline(PROMPT)` が端末に出力 |
| 8 | ヒアドキュメントプロンプト (`"ウラ> "`) | 端末 (TTY) | `readline("ウラ> ")` が端末に出力 |
| 9 | `^C\n` (Ctrl+C 時) | stdout | `handler_interactive` → `write(STDOUT_FILENO, "^C\n", 3)` |
| 10 | `\n` (ヒアドキュメント中 Ctrl+C 時) | stdout | `heredoc_parent` → `write(STDOUT_FILENO, "\n", 1)` |
| 11 | `rl_redisplay()` による端末再描画 | 端末 (TTY) | `handler_interactive` → `rl_on_new_line()` + `rl_replace_line("", 0)` + `rl_redisplay()` |
| 12 | エラーメッセージ各種 | stderr | `builtin_cd`, `builtin_export`, `builtin_exit`, `child_cmd_not_found`, `child_exec_failed`, `lexer.c`, `perror()` 等 |
| 13 | `exit` (シェル正常終了通知) | stderr | `read_prompt:47` → `ft_putendl_fd("exit", STDERR_FILENO)` |
| 14 | `Quit` / `Quit (core dumped)` (SIGQUIT 時) | stderr | `calculate_exit_status_quit` |
| 15 | 外部コマンドの stderr | stderr | `execve` 経由で子プロセスが書き込む |
| 16 | リダイレクト `>` による上書きファイル | ファイルシステム | `handle_output` → `open(O_WRONLY\|O_CREAT\|O_TRUNC)` + `dup2` |
| 17 | リダイレクト `>>` による追記 | ファイルシステム | `handle_output` → `open(O_WRONLY\|O_CREAT\|O_APPEND)` + `dup2` |
| 18 | ヒアドキュメント一時ファイル作成 (`/tmp/.minishell_heredoc_*`) | ファイルシステム | `heredoc_child` → `open(O_WRONLY\|O_CREAT\|O_TRUNC, 0600)` + `ft_putendl_fd(line, fd)` |
| 19 | ヒアドキュメント一時ファイル削除 | ファイルシステム | `redirect.c:65` → `unlink(redir->filename)` (実行後) / `heredoc.c:122` → `unlink(tmp_filename)` (SIGINT 時) |
| 20 | 外部コマンド子プロセスの起動 | OS プロセステーブル | `exec_external` → `fork()` + `execve()` |
| 21 | パイプ子プロセス (左右) の起動 | OS プロセステーブル | `exec_pipeline` → `fork()` × 2 + `execve()` |
| 22 | サブシェル子プロセスの起動 | OS プロセステーブル | `exec_subshell` → `fork()` |
| 23 | ヒアドキュメント読み取り子プロセスの起動 | OS プロセステーブル | `read_heredoc` → `fork()` |
| 24 | パイプへのデータ出力 | カーネルパイプバッファ | 左子プロセス: `dup2(fd[1], STDOUT_FILENO)` → `exec_ast(left)` の stdout → `fd[1]` |
| 25 | カレントディレクトリの変更 | OS プロセス状態 | `builtin_cd` → `chdir(path)` |
| 26 | 子プロセスへの環境変数引き渡し | 子プロセスの環境 | `exec_child` → `env_list_to_array()` → `execve(path, args, envp)` の第3引数 |
| 27 | readline 履歴への追加 | readline 内部状態 | `read_prompt:56` → `add_history(line)` |
| 28 | readline 履歴の全消去 | readline 内部状態 | `main:30`, `cleanup_and_exit` → `rl_clear_history()` |
| 29 | 端末属性の書き戻し | 端末 (TTY) | `heredoc.c:71` → `tcsetattr(STDIN_FILENO, TCSANOW, &term)` |
| 30 | プロセス終了ステータス | OS プロセステーブル | `cleanup_and_exit` → `exit(status)` / `main:33` → `return(exit_status)` |

---

## 動作環境

| # | 要素 | 影響内容 |
|---|------|----------|
| 1 | OS カーネル (プロセスモデル) | `fork`, `execve`, `waitpid`, `pipe`, `dup2` の動作。プロセス生成・パイプ・fd 継承規則が OS 依存 |
| 2 | OS カーネル (シグナル配送) | `SIGINT`, `SIGQUIT` の配送タイミング・`SA_RESTART` の有無などが OS 実装依存 |
| 3 | ファイルシステム | `access()`, `stat()`, `opendir()`/`readdir()`, リダイレクト先ファイルの存在・パーミッション・PATH 上の実行ファイル探索 |
| 4 | `/tmp` の存在と書き込み権限 | ヒアドキュメント一時ファイル作成に必要。存在しないか書き込み不可なら heredoc が失敗する |
| 5 | `/dev/urandom` の存在 | ヒアドキュメント一時ファイル名生成に使用。存在しない場合は生成失敗 |
| 6 | TTY / ターミナルエミュレータ | `isatty()`, `tcgetattr()`/`tcsetattr()`, readline の表示制御。インタラクティブ動作判定に影響 |
| 7 | `umask` | `open(..., 0644)`, `open(..., 0600)` での実際のパーミッションが umask によって変わる |
| 8 | `PATH_MAX` 定数 | `cd.c`, `pwd.c` の `char cwd[PATH_MAX]` のスタックバッファサイズが OS 定義値に依存 |

---

## 外部モジュール

| # | モジュール | リンク形式 | 使用箇所 | 影響内容 |
|---|-----------|-----------|----------|----------|
| 1 | GNU readline (`-lreadline`) | 動的リンク | `readline()`, `add_history()`, `rl_clear_history()`, `rl_on_new_line()`, `rl_replace_line()`, `rl_redisplay()`, `rl_catch_signals` | プロンプト表示・行編集・履歴管理の挙動全体。バージョンや設定により動作が変わる |
| 2 | libft (`libft.a`) | 静的リンク (自作) | `ft_*` 関数全般 | 文字列処理・メモリ操作の実装が本体の挙動に直結。バグがあればシステム全体に波及 |
| 3 | libc (標準 C ライブラリ) | 動的リンク | `malloc`/`free`, `open`/`close`/`dup2`, `fork`/`execve`/`waitpid`, `signal`, `perror`, `strerror`, `stat`, `access` 等 | システムコール・メモリ管理・エラー報告の根幹。実装 (glibc / musl 等) により挙動差がある |



## 分類まとめ
```
  外部モジュール
    ├─ GNU readline   (動的リンク)
    ├─ libft          (静的リンク、自作)
    └─ libc           (動的リンク)

  動作環境        
    ├─ OS カーネル    (プロセス・シグナル)
    ├─ ファイルシステム (/tmp, /dev/urandom, PATH 上の実行ファイル)
    ├─ TTY / ターミナル
    └─ umask / PATH_MAX などの OS 定義値
```
