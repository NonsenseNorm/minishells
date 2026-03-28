# Resource Management — claude/

各モジュールが所有・管理するリソースの一覧です。  
「所有」とは **確保した側がそのまま解放責任を持つ** ことを意味します。

---

## 凡例

| 種別 | 内容 |
|------|------|
| **ヒープ** | `malloc`/`ft_strdup`/`ft_strjoin` 等で確保した動的メモリ |
| **アリーナ** | `ms_alloc`/`ms_strdup` で確保したアリーナ管理メモリ |
| **FD** | ファイルディスクリプタ（pipe, open, dup 等） |
| **プロセス** | fork した子プロセス |
| **端末状態** | termios 設定・echoctl フラグ |
| **シグナル** | signal(2) で登録したハンドラ |
| **readline** | readline ライブラリが内部管理する履歴・バッファ |
| **グローバル変数** | `g_sig` への書き込み権 |

---

## 1. mem モジュール (`src/mem/mem.c`)

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **ヒープ** (ブロック) | `ms_alloc` → `new_block` → `malloc` | `mem_reset` / `mem_pop` → `free` | アリーナ本体のブロック列 |

- `mem_init` : ブロック列を空に初期化（確保なし）
- `mem_mark` / `mem_pop` : スタックポインタのセーブ/ロール-バック。コマンド1本分の一時メモリをまとめて解放するために使う
- `mem_reset` : アリーナ全体を解放。`main` 終了時に呼ぶ

**所有範囲**: アリーナブロック列のヒープメモリ全体

---

## 2. env モジュール (`src/env/env.c`, `src/env/env_getset.c`)

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **ヒープ** (`env->arr`) | `env_init` → `malloc` (配列) + `ft_strdup` (各要素) | `env_free` | `"KEY=VALUE"` 文字列配列 |
| **ヒープ** (各エントリ) | `env_set` → `make_kv` → `malloc`+`ft_strdup` | `env_set`/`env_unset` で古い要素を `free` | 値変更時は古いポインタを解放 |
| **ヒープ** (配列拡張) | `env_grow` → `realloc` | `env_free` で配列ごと解放 | 容量不足時に2倍拡張 |

- `env_get` : 所有権を移動しない（読み取りのみ）
- `env_unset` : 該当要素を `free` し、末尾要素で上書き。配列サイズは縮小しない

**所有範囲**: `t_env.arr` が指す配列と、その各要素文字列

---

## 3. signal モジュール (`src/signal/signal.c`)

| リソース | 確保（登録） | 解放（リセット） | 備考 |
|----------|-------------|-----------------|------|
| **シグナルハンドラ** SIGINT | `sig_set_interactive` / `sig_set_heredoc` | `sig_set_exec_parent` (SIG_IGN) / `sig_set_exec_child` (SIG_DFL) | |
| **シグナルハンドラ** SIGQUIT | `sig_set_*` 各関数で SIG_IGN or SIG_DFL | — | |
| **グローバル変数** `g_sig` | 書き込み: `sig_handler_interactive`, `sig_handler_record` | `sig_set_interactive` / `sig_set_heredoc` で 0 にリセット | 読み取りは loop.c, heredoc.c |

シグナルモジュール自体はヒープ・FD・プロセスを管理しない。  
ハンドラの副作用として readline バッファをクリア (`rl_replace_line`・`rl_done=1`) するが、  
readline 内部メモリの所有権は readline ライブラリが持つ。

**所有範囲**: signal(2) 登録ハンドラ、`g_sig` の書き込み権

---

## 4. lexer モジュール (`src/lexer/`)

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **アリーナ** (`t_token` リスト) | `lex_line` → `ms_alloc` / `ms_strdup` | `mem_pop` (呼び出し元 loop.c) | トークン列全体 |

- `t_token.value` はアリーナ上の文字列。個別 `free` 不要
- `lex_line` はアリーナにアロケートするため、呼び出し元が `mem_mark`/`mem_pop` で管理

**所有範囲**: なし（アリーナはmemモジュール所有、mark/popはloop.cが管理）

---

## 5. parser モジュール (`src/parser/`)

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **アリーナ** (`t_pipeline`, `t_cmd[]`, `t_redirect` リスト) | `parse_pipeline` → `ms_alloc` / `ms_strdup` | `mem_pop` (呼び出し元 loop.c) | パイプライン構造体全体 |

- `t_redirect.target` もアリーナ文字列
- heredoc の `quoted` フラグをここで確定する（字句段階で `has_quote` を呼び、フラグをセット）

**所有範囲**: なし（アリーナはmemモジュール所有）

---

## 6. expand モジュール (`src/expand/`)

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **ヒープ** (展開後文字列) | `expand_word` → `malloc`/`ft_strjoin` 等 | 呼び出し元が `free` | `expand_pipeline` は `ms_strdup` でアリーナにコピー後、一時ヒープを解放 |
| **アリーナ** (展開後 argv 等) | `expand_pipeline` → `ms_alloc` / `ms_strdup` | `mem_pop` (loop.c) | |

- `expand_word` はヒープに一時文字列を返す。呼び出し元責任で `free`
- `heredoc_write` も `expand_word` を呼び、返り値を `free`

**所有範囲**: `expand_word` が返すヒープ文字列（呼び出し元に所有権を譲渡）

---

## 7. exec モジュール (`src/exec/exec.c`, `src/exec/pipeline.c`)

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **FD** (stdin/stdout 退避) | `exec_single_parent` → `dup` | 同関数内で `dup2` + `close` | 親プロセスでビルトインを実行する場合 |
| **FD** (パイプ) | `exec_forked_pipeline` → `pipe` | 同関数内でループ中に `close` | 読み端は次の子へ渡し、書き端は即 close |
| **プロセス** | `exec_forked_pipeline` → `fork` | `wait_all` → `waitpid` | 全子プロセスを回収してから戻る |
| **シグナルハンドラ** | `sig_set_exec_parent` (exec前) → `sig_set_interactive` (wait後) | — | exec 中は SIGINT/SIGQUIT を SIG_IGN |
| **アリーナ** (`pid_t[]`) | `ms_alloc` | `mem_pop` (loop.c) | |

- `child_exec` は `execve` を呼ぶかエラー時 `exit` するため、FD を閉じる責任は exec される新プロセスに移る
- `exec_single_parent` は FD 退避→ビルトイン実行→FD 復元をすべて自分で行う

**所有範囲**: パイプ FD（親側）、子プロセスのライフサイクル、exec 中のシグナルハンドラ

---

## 8. redirect モジュール (`src/exec/redirect.c`)

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **FD** (open/heredoc) | `apply_redirects` → `open` or `heredoc_fd` | `dup2` 後に `close` | dup2 でstdin/stdout に置き換えた後、元 FD を閉じる |

- heredoc で `g_sig == SIGINT` が検出された場合（`heredoc_fd` が -1 を返す）は FD を開かず即返る
- 元の stdin/stdout FD はこのモジュールでは退避しない（`exec_single_parent` が上位で退避）

**所有範囲**: open した一時 FD（dup2 後に close）

---

## 9. heredoc モジュール (`src/exec/heredoc.c`)

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **FD** (パイプ読み端) | `heredoc_fd` → `pipe` | 呼び出し元 (`apply_redirects`) が使用後 close | 書き端は heredoc_fd 内で close |
| **FD** (パイプ書き端) | `pipe` | `heredoc_fd` 内で close（入力終了後） | |
| **ヒープ** (各行) | `read_heredoc_line` → `ft_strdup`/`ft_strjoin` | `heredoc_write` 内で `free` / ループ末尾で `free` | |
| **シグナルハンドラ** | `sig_set_heredoc` | `sig_set_interactive` (heredoc 終了後) | heredoc 中のみ record ハンドラ |
| **`g_sig`** | `sig_handler_record` が書き込み | `sig_set_heredoc` で 0 にリセット | SIGINT 検出に使用 |

- SIGINT で中断した場合: パイプ読み端を close し、`sh->exit_code = 130` をセット、-1 を返す
- 正常終了時: パイプ読み端を呼び出し元に渡す（所有権移動）

**所有範囲**: パイプ書き端（常に自分で close）、行バッファ（一時ヒープ）、heredoc 中のシグナルハンドラ

---

## 10. path モジュール (`src/exec/path.c`)

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **ヒープ** (パス文字列) | `find_exec_path` → `malloc`/`ft_strjoin` 等 | 呼び出し元 (`child_exec`) は `execve` に渡すため OS が管理 | execve 失敗時は `child_exec` が `exit` するためリークは問題なし |

**所有範囲**: `find_exec_path` が返すヒープ文字列（呼び出し元に所有権を譲渡）

---

## 11. builtin モジュール (`src/builtin/`)

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **ヒープ** (`bi_cd` の getcwd) | `bi_cd` → `getcwd(NULL, 0)` | `bi_cd` 内で `free` | OLDPWD 更新用の一時文字列 |
| **ヒープ** (`bi_export` の整形文字列) | `bi_export` → `ft_strjoin`等 | `bi_export` 内で `free` | 引数なしで一覧表示する場合 |
| **env エントリ** | `bi_export` → `env_set` | `env_free` (終了時) | 新しい KEY=VALUE の追加 |
| **env エントリ** | `bi_unset` → `env_unset` | 即 `free` | 既存エントリの削除 |

**所有範囲**: 一時ヒープ（関数内で完結）、env への変更は env モジュールに委譲

---

## 12. loop モジュール (`src/core/loop.c`)

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **ヒープ** (readline 行) | `readline` | `free(line)` （ループ末尾） | readline が malloc する文字列 |
| **readline 履歴** | `add_history(line)` | `clear_history()` (main 終了時) | readline 内部管理 |
| **アリーナ (mark/pop)** | `mem_mark` | `mem_pop` | コマンド1本分のアリーナ領域を管理 |
| **シグナルハンドラ** | `sig_set_interactive` (ループ開始前・exec後) | — | interactive モード用ハンドラを維持 |
| **`g_sig`** 読み取り | — | — | `g_sig == SIGINT` で SIGINT をフラッシュ処理 |

**所有範囲**: readline が返す行バッファ（毎ループ free）、mark/pop によるアリーナ管理権

---

## 13. main / init (`src/core/main.c`)

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **ヒープ** (`t_shell`) | `main` → `malloc` (スタック変数のためヒープではない) | — | `t_shell sh` はスタック変数 |
| **アリーナ** (`t_mem`) | `mem_init` | `mem_reset` (終了時) | |
| **env** | `env_init` | `env_free` (終了時) | environ のコピー |
| **readline 履歴** | — | `clear_history` (終了時) | loop.c が add した履歴をまとめて解放 |
| **端末状態** | `ms_term_disable_echoctl` → `tcgetattr`/`tcsetattr` | `restore_terminal` → `tcsetattr` で元設定を復元 | echoctl を無効化し ^C 表示を抑制 |
| **シグナルハンドラ** (初期) | `sig_set_interactive` (ms_loop 内で初回設定) | — | |
| **グローバル変数** `g_sig` | `main.c` で `volatile sig_atomic_t g_sig = 0` と定義 | — | 定義はここ、書き込みは signal.c |

**所有範囲**: アリーナ全体・env 全体・readline 履歴・端末状態の最終的な解放責任

---

## リソースライフサイクル早見表

```
main 起動
  └─ mem_init          [アリーナ初期化]
  └─ env_init          [env 確保]
  └─ ms_term_disable_echoctl  [端末状態変更]
  └─ ms_loop
       └─ sig_set_interactive  [シグナルハンドラ登録]
       └─ (各コマンドループ)
            └─ readline        [行バッファ確保] ──> free(line)
            └─ add_history     [履歴追加]
            └─ mem_mark        [アリーナマーク]
            └─ lex_line        [トークン: アリーナ]
            └─ parse_pipeline  [AST: アリーナ]
            └─ expand_pipeline [展開: アリーナ+一時ヒープ解放]
            └─ exec_pipeline
                 ├─ (parent builtin) exec_single_parent
                 │    └─ dup/dup2/close  [FD 退避→復元]
                 └─ exec_forked_pipeline
                      └─ sig_set_exec_parent  [親: SIGINT/QUIT 無視]
                      └─ pipe / fork
                           └─ (child) child_exec → execve / exit
                      └─ close(pipe fds)
                      └─ wait_all → waitpid  [子プロセス回収]
                      └─ sig_set_interactive  [ハンドラ復元]
            └─ mem_pop         [アリーナ解放]
  └─ clear_history     [履歴解放]
  └─ env_free          [env 解放]
  └─ mem_reset         [アリーナ全解放]
  └─ restore_terminal  [端末状態復元]
```
