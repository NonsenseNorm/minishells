# minishell に影響を与えうる要因一覧（Linux 環境）

ハードウェア（ドライバ以下）は対象外。

---

## 1. Linux カーネル

| 要因 | 影響する処理 | 具体的影響 |
|------|-------------|-----------|
| カーネルバージョン | `fork`, `execve`, `pipe`, `signal`, `waitpid` | バージョンによる syscall セマンティクスの差異 |
| パイプバッファサイズ（デフォルト 64KB、`/proc/sys/fs/pipe-max-size` で変更可） | `pipe()` + `dup2` によるパイプ処理 | バッファを超えるデータを書き込む際にブロックが発生し、デッドロックの可能性 |
| シグナル配送セマンティクス（`SA_RESTART` の有無） | `readline()` 中の SIGINT 処理 | システムコールが interrupted で失敗するかどうかが変わる |
| OOM killer | `fork()`, `malloc()` | メモリ不足時にプロセスが強制終了される |
| `/proc/sys/kernel/pid_max` | `fork()` の連続呼び出し | PID が枯渇すると fork() が失敗する |

---

## 2. ファイルシステム

| 要因 | 影響する処理 | 具体的影響 |
|------|-------------|-----------|
| `/tmp` の存在・容量・マウント状態 | `open("/tmp/.minishell_heredoc_*")` | 存在しない・容量不足・読み取り専用なら heredoc が失敗する |
| `/dev/urandom` の存在 | `open("/dev/urandom", O_RDONLY)` | 存在しない場合、一時ファイル名生成が失敗し heredoc 全体が動作不能になる |
| `readdir()` の返す順序（ファイルシステム種類依存） | ワイルドカード展開 `expand_wildcard()` | ext4・tmpfs・XFS 等によってエントリ列挙順が異なり、`sort_str_array` 前の収集順が変わる |
| リダイレクト対象ファイルの存在・パーミッション・所有権 | `open()` によるリダイレクト | 存在しない・権限なし → `open()` が ENOENT / EACCES で失敗 |
| PATH 上の実行ファイルの存在・実行権限 | `access(path, X_OK)` → `execve()` | 存在しない or 実行権限なし → "command not found" / "Permission denied" |
| inode 枯渇 | `open(O_CREAT)` によるファイル作成 | リダイレクト出力ファイル・heredoc 一時ファイルの作成が失敗する |
| シンボリックリンク | `chdir()`, `getcwd()`, `execve()` パス解決 | リンク先の状態によって chdir の成否・getcwd の返す値が変わる |
| ファイルシステムの空き容量 | `>` / `>>` リダイレクト書き込み | 空きがない場合、書き込みが途中で失敗する |

---

## 3. プロセス・リソース制限（ulimit）

| 要因 | 影響する処理 | 具体的影響 |
|------|-------------|-----------|
| `ulimit -n`（最大オープン fd 数） | `pipe()`, `open()`, `dup2()`, `fork()` | fd が枯渇すると pipe / open / dup2 が失敗し、リダイレクト・パイプ・heredoc が動作不能になる |
| `ulimit -u`（最大プロセス数） | `fork()` | 上限到達で fork() が EAGAIN で失敗し、外部コマンド・パイプ・サブシェル・heredoc すべてが動作不能になる |
| `ulimit -s`（スタックサイズ） | `char cwd[PATH_MAX]`（スタック上のバッファ） | 極端に小さいと、cd / pwd のスタックフレームでスタックオーバーフローの可能性 |
| `ulimit -v`（仮想メモリ上限） | `malloc()` 全般 | 上限到達で malloc が NULL を返し、メモリ確保失敗が連鎖する |

---

## 4. プロセス実行環境

| 要因 | 影響する処理 | 具体的影響 |
|------|-------------|-----------|
| 親プロセスから引き継いだ環境変数（`envp`） | `env_init(envp)` | `HOME`, `PATH`, `OLDPWD` 等の初期値がすべて親プロセス依存。`PATH` がなければ外部コマンド探索が `./cmd` のみになる |
| 親プロセスから引き継いだ open fd（fd リーク） | `pipe()`, `open()` の fd 番号割り当て | 継承 fd が多いほど低番号 fd が埋まり、`ulimit -n` に近づく |
| `umask` | `open(..., 0644)`, `open(..., 0600)` | リダイレクト出力ファイルや heredoc 一時ファイルの実際のパーミッションが変わる |
| プロセスグループ・セッション・制御端末 | `SIGINT` / `SIGQUIT` の配送 | foreground プロセスグループ全体にシグナルが届くため、minishell と子プロセスの両方がシグナルを受け取る |
| 起動時のカレントディレクトリ（CWD） | 相対パス解決・`opendir(".")`（ワイルドカード）・`chdir()` の基点 | 相対パスの解釈・ワイルドカードのマッチ対象がすべて起動時 CWD 依存 |

---

## 5. TTY・端末エミュレータ

| 要因 | 影響する処理 | 具体的影響 |
|------|-------------|-----------|
| 端末エミュレータの種類（xterm / gnome-terminal / iTerm 等） | readline によるエスケープシーケンス出力 | 端末ごとにエスケープシーケンスの解釈が異なり、プロンプト表示・再描画がずれる可能性 |
| `TERM` 環境変数 | readline の端末制御 | readline が terminfo を参照して端末能力を判断するため、不正な値だと表示が崩れる |
| 端末サイズ（列数 `COLUMNS` / 行数 `LINES`） | readline のプロンプト折り返し・再描画（`rl_redisplay()`） | 端末幅を超えるプロンプトや出力で再描画位置がずれる |
| ロケール・文字コード（`LANG`, `LC_ALL`, `LC_CTYPE`） | readline のマルチバイト文字幅計算・日本語プロンプト (`"何か打ち込んでみろッ！"`, `"ウラ> "`) の表示 | UTF-8 以外の環境や `LC_CTYPE=C` では文字境界の認識が誤り、カーソル位置がずれる |
| ブラケットペーストモードの有効/無効 | `readline()` の返す文字列・`exec_multiline()` | 無効時は貼り付けた複数行が `\n` を含まず 1 行として渡され、`exec_multiline` の分岐に入らない |
| `stty` 設定（`tcgetattr` の返す値） | `tcgetattr()` → heredoc 前後の端末属性保存・復元 | INTR/QUIT キーの割り当てや ICRNL 等のフラグが異なると、heredoc 中の Ctrl+C の検知・端末属性の復元結果が変わる |
| `isatty(STDIN_FILENO)` の結果 | `builtin_exit` の "exit" メッセージ出力判定 | stdin がパイプや非 TTY の場合、exit 時の "exit" メッセージが出力されない |

---

## 6. セキュリティモジュール

| 要因 | 影響する処理 | 具体的影響 |
|------|-------------|-----------|
| SELinux / AppArmor | `execve()`, `open()`, `chdir()`, `fork()` | ポリシー違反の操作が EACCES / EPERM で拒否される。コマンド実行・リダイレクト・cd が予期せず失敗する可能性 |
| Seccomp（syscall フィルタ） | `fork()`, `execve()`, `open()`, `pipe()` 等 | コンテナや制限環境で特定 syscall が禁止されている場合、フィルタに引っかかった時点でプロセスが強制終了される |

---

## 7. コンテナ・仮想化環境

| 要因 | 影響する処理 | 具体的影響 |
|------|-------------|-----------|
| PID namespace | `fork()` で生成される PID 値・プロセスグループ | コンテナ内では PID 1 が init でないため、孤立した子プロセスのシグナル処理やゾンビ回収の動作が通常と異なる |
| マウント namespace | `/tmp`, `/dev/urandom`, PATH 上のコマンド | ホストと異なるファイルシステムビューになり、heredoc・一時ファイル・コマンド探索の結果が変わる |

---

## 8. 外部モジュールのバージョン・実装

| 要因 | 影響する処理 | 具体的影響 |
|------|-------------|-----------|
| GNU readline のバージョン | `rl_catch_signals`, `rl_replace_line()`, `rl_redisplay()`, `add_history()` | バージョンによって API の動作・シグナルとのインタラクションが異なる |
| libc の実装（glibc / musl）とバージョン | `malloc`/`free`, `fork`, `execve`, `signal`, `strerror`, `perror` 等 | malloc のアロケーション挙動・errno の設定タイミング・`strerror` のメッセージ文言・シグナル処理の差異 |
| libft の実装（自作） | `ft_*` 関数を使う全処理 | バグがあれば文字列処理・メモリ管理を通じてシステム全体の挙動に波及する |

---

## 9. 外部コマンド

| 要因 | 影響する処理 | 具体的影響 |
|------|-------------|-----------|
| `PATH` の内容と各ディレクトリの状態 | `get_cmd_path()` による `access(path, X_OK)` ループ | PATH に存在しない・実行不可なら "command not found"。PATH の順序でどのバイナリが選ばれるかが決まる |
| 外部コマンドの動作・バージョン | `execve(path, args, envp)` 以降 | コマンドの終了ステータス・stdout/stderr 出力・シグナル送信が `$?` や画面出力に直接影響する |
| 外部コマンドが送るシグナル | `waitpid()` の `status` (`WIFSIGNALED`) | 子プロセスがシグナルで終了した場合、`$?` が `128 + signum` になり後続の `&&` / `\|\|` 分岐に影響する |

---

## 要因カテゴリまとめ

```
Linux カーネル
  ├─ カーネルバージョン・syscall セマンティクス
  ├─ パイプバッファサイズ
  ├─ シグナル配送セマンティクス
  └─ OOM killer / PID 上限

ファイルシステム
  ├─ /tmp・/dev/urandom の状態
  ├─ readdir 順序（FS 種類依存）
  ├─ ファイルの存在・権限・inode・容量
  └─ シンボリックリンク

プロセス実行環境
  ├─ ulimit（fd数・プロセス数・スタック・メモリ）
  ├─ 引き継ぎ envp・open fd・umask
  ├─ プロセスグループ・制御端末
  └─ 起動時 CWD

TTY・端末
  ├─ 端末エミュレータ種類・TERM変数
  ├─ 端末サイズ（COLUMNS/LINES）
  ├─ ロケール・文字コード
  ├─ ブラケットペーストモード
  ├─ stty 設定
  └─ isatty 判定

セキュリティ・仮想化
  ├─ SELinux / AppArmor
  ├─ Seccomp
  ├─ PID / マウント namespace（コンテナ）
  └─ umask

外部モジュール
  ├─ GNU readline バージョン
  ├─ libc 実装・バージョン
  └─ libft 実装（自作）

外部コマンド
  ├─ PATH の内容・各コマンドの存在・権限
  ├─ コマンドの動作・終了ステータス
  └─ コマンドが送るシグナル
```

---
/btw さっきのaffecting_factors.mdにはSIGPIPEが起こったときについては書かれてなかった気がするけどこれは問題にならないの？bashと違う挙動するなら問題とする。

    問題になります。記載すべきでした。

    ---
    SIGPIPE の問題

    bash の動作

    - bash 自身は起動時に SIGPIPE = SIG_IGN を設定する（シェルプロセス自体がパイプ切断で死なないように）
    - 子プロセスには明示的に SIGPIPE = SIG_DFL にリセットして渡す

    この minishell の動作

    set_signal_child() を見ると:

    void set_signal_child(void)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
    }

    SIGPIPE は一切設定していない。

    つまり：

    1. minishell 自身: SIGPIPE を SIG_IGN にしていないため、minishell がパイプの書き込み側になった場合（例: echo foo | ./minishell）、読み取り側が閉じると minishell プロセス自体が死ぬ可能性がある
    2. 子プロセス: set_signal_child() で SIGPIPE をリセットしないため、minishell を起動した親プロセスの SIGPIPE 設定をそのまま引き継ぐ

    具体的に差が出るケース

    yes | head -1 のようなコマンドを、SIGPIPE = SIG_IGN な環境（nohup 経由など）から起動した minishell で実行した場合：

    - bash: 子プロセスに明示的に SIG_DFL を渡すので yes は SIGPIPE で正常終了
    - この minishell: yes が SIG_IGN を引き継ぐため SIGPIPE を無視し、書き込みが EPIPE エラーを返し続けるが終了せず、無限ループになる可能性がある

    結論

    起動環境の SIGPIPE 設定に動作が依存するという点で bash と異なる挙動をしうるため、affecting_factors.md の「プロセス実行環境」の引き継ぎ fd・シグナル設定の項目に追記すべき要因です。
