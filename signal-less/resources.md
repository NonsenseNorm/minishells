# Resource Management — signal-less/claude/

signal モジュールと端末制御を取り除いたバージョンのリソース管理一覧です。  
`claude/resources.md` との差分を中心に記述します。

---

## 凡例

| 種別 | 内容 |
|------|------|
| **ヒープ** | `malloc`/`ft_strdup`/`ft_strjoin` 等で確保した動的メモリ |
| **アリーナ** | `ms_alloc`/`ms_strdup` で確保したアリーナ管理メモリ |
| **FD** | ファイルディスクリプタ（pipe, open, dup 等） |
| **プロセス** | fork した子プロセス |
| **readline** | readline ライブラリが内部管理する履歴・バッファ |

> **signal-less 版では以下が存在しない:**  
> - `g_sig` グローバル変数  
> - `sig_set_*` ハンドラ管理  
> - `term_saved`/`term_orig`（termios 端末状態）  
> - `ms_term_disable_echoctl`/`restore_terminal`  
> - `t_shell.term_saved`/`t_shell.term_orig` フィールド

---

## 1. mem モジュール (`src/mem/mem.c`)

`claude/` 版と同一。

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **ヒープ** (ブロック) | `ms_alloc` → `new_block` → `malloc` | `mem_reset` / `mem_pop` → `free` | アリーナ本体のブロック列 |

**所有範囲**: アリーナブロック列のヒープメモリ全体

---

## 2. env モジュール (`src/env/env.c`, `src/env/env_getset.c`)

`claude/` 版と同一。

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **ヒープ** (`env->arr`) | `env_init` → `malloc` + `ft_strdup` | `env_free` | `"KEY=VALUE"` 文字列配列 |
| **ヒープ** (各エントリ) | `env_set` → `make_kv` | `env_set`/`env_unset` で古い要素を `free` | |
| **ヒープ** (配列拡張) | `env_grow` → `realloc` | `env_free` | 2倍拡張 |

**所有範囲**: `t_env.arr` が指す配列と各要素文字列

---

## 3. signal モジュール

**このバージョンには signal モジュールは存在しない。**

- `g_sig` は宣言・定義されない
- シグナルハンドラは一切登録しない（すべてデフォルト動作）
- SIGINT でプロセスが終了することを許容する設計

---

## 4. lexer モジュール (`src/lexer/`)

`claude/` 版と同一。

| リソース | 確保 | 解放 |
|----------|------|------|
| **アリーナ** (`t_token` リスト) | `lex_line` → `ms_alloc`/`ms_strdup` | `mem_pop` (loop.c) |

**所有範囲**: なし（アリーナは mem モジュール所有）

---

## 5. parser モジュール (`src/parser/`)

`claude/` 版と同一。

| リソース | 確保 | 解放 |
|----------|------|------|
| **アリーナ** (`t_pipeline`, `t_cmd[]`, `t_redirect` リスト) | `parse_pipeline` → `ms_alloc`/`ms_strdup` | `mem_pop` (loop.c) |

**所有範囲**: なし（アリーナは mem モジュール所有）

---

## 6. expand モジュール (`src/expand/`)

`claude/` 版と同一。

| リソース | 確保 | 解放 |
|----------|------|------|
| **ヒープ** (展開後文字列) | `expand_word` → `malloc`/`ft_strjoin` 等 | 呼び出し元が `free` |
| **アリーナ** (展開後 argv 等) | `expand_pipeline` → `ms_alloc`/`ms_strdup` | `mem_pop` (loop.c) |

**所有範囲**: `expand_word` が返すヒープ文字列（呼び出し元に所有権を譲渡）

---

## 7. exec モジュール (`src/exec/exec.c`, `src/exec/pipeline.c`)

`claude/` 版との差分: **シグナルハンドラの切り替えがない**。

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **FD** (stdin/stdout 退避) | `exec_single_parent` → `dup` | 同関数内で `dup2` + `close` | |
| **FD** (パイプ) | `exec_forked_pipeline` → `pipe` | ループ中に `close` | |
| **プロセス** | `exec_forked_pipeline` → `fork` | `wait_all` → `waitpid` | |
| **アリーナ** (`pid_t[]`) | `ms_alloc` | `mem_pop` (loop.c) | |

> `claude/` 版と異なり:
> - `exec_forked_pipeline` に `sig_set_exec_parent()` 呼び出しなし
> - `wait_all` 後の `sig_set_interactive()` 呼び出しなし
> - `child_exec` に `sig_set_exec_child()` 呼び出しなし
>
> 子プロセスはシグナルハンドラを継承したまま `execve` する

**所有範囲**: パイプ FD（親側）、子プロセスのライフサイクル

---

## 8. redirect モジュール (`src/exec/redirect.c`)

`claude/` 版との差分: **`g_sig == SIGINT` による中断チェックがない**。

| リソース | 確保 | 解放 |
|----------|------|------|
| **FD** (open/heredoc) | `apply_redirects` → `open` or `heredoc_fd` | `dup2` 後に `close` |

**所有範囲**: open した一時 FD（dup2 後に close）

---

## 9. heredoc モジュール (`src/exec/heredoc.c`)

`claude/` 版との差分: **シグナル処理・`g_sig` チェックがない**。  
SIGINT を受けると `read` が -1 を返し、EOF と同じ扱いで heredoc が終了する。

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **FD** (パイプ読み端) | `heredoc_fd` → `pipe` | 呼び出し元 (`apply_redirects`) が使用後 close | |
| **FD** (パイプ書き端) | `pipe` | `heredoc_fd` 内で close | |
| **ヒープ** (各行) | `read_heredoc_line` → `ft_strdup`/`ft_strjoin` | `heredoc_write` 内で `free` / ループ末尾で `free` | |

> `claude/` 版と異なり:
> - `sig_set_heredoc()` / `sig_set_interactive()` 呼び出しなし
> - `g_sig == SIGINT` チェックなし → SIGINT 検出時の `-1` 返却・`sh->exit_code = 130` セットなし
> - 正常/異常を問わず常に `p[0]` を返す（EOF で自然終了）

**所有範囲**: パイプ書き端（常に自分で close）、行バッファ（一時ヒープ）

---

## 10. path モジュール (`src/exec/path.c`)

`claude/` 版と同一。

| リソース | 確保 | 解放 |
|----------|------|------|
| **ヒープ** (パス文字列) | `find_exec_path` → `malloc`/`ft_strjoin` 等 | 呼び出し元 (`child_exec`) は `execve` に渡す |

**所有範囲**: `find_exec_path` が返すヒープ文字列（呼び出し元に所有権を譲渡）

---

## 11. builtin モジュール (`src/builtin/`)

`claude/` 版と同一。

| リソース | 確保 | 解放 |
|----------|------|------|
| **ヒープ** (`bi_cd` の getcwd) | `getcwd(NULL, 0)` | `bi_cd` 内で `free` |
| **ヒープ** (`bi_export` 整形) | `ft_strjoin` 等 | `bi_export` 内で `free` |
| **env エントリ** | `env_set` / `env_unset` | env モジュールが管理 |

**所有範囲**: 一時ヒープ（関数内で完結）

---

## 12. loop モジュール (`src/core/loop.c`)

`claude/` 版との差分: **シグナル関連処理がすべて削除されている**。

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **ヒープ** (readline 行) | `readline` | `free(line)` （ループ末尾） | |
| **readline 履歴** | `add_history(line)` | `clear_history()` (main 終了時) | |
| **アリーナ (mark/pop)** | `mem_mark` | `mem_pop` | コマンド1本分のアリーナ領域 |

> `claude/` 版と異なり:
> - `sig_set_interactive()` 呼び出しなし
> - `g_sig == SIGINT` の flush 処理なし
> - SIGINT を受けたとき readline は NULL を返し、ループが終了する（exit 扱い）

**所有範囲**: readline が返す行バッファ（毎ループ free）、mark/pop によるアリーナ管理権

---

## 13. main / init (`src/core/main.c`)

`claude/` 版との差分: **端末状態管理・`g_sig` 定義がない**。

| リソース | 確保 | 解放 | 備考 |
|----------|------|------|------|
| **アリーナ** (`t_mem`) | `mem_init` | `mem_reset` (終了時) | |
| **env** | `env_init` | `env_free` (終了時) | environ のコピー |
| **readline 履歴** | — | `clear_history` (終了時) | loop.c が add した履歴をまとめて解放 |

> `claude/` 版と異なり:
> - `g_sig` の定義なし（グローバル変数なし）
> - `ms_term_disable_echoctl` / `restore_terminal` 呼び出しなし
> - `t_shell` に `term_saved`/`term_orig` フィールドなし → `tcgetattr`/`tcsetattr` 不使用

**所有範囲**: アリーナ全体・env 全体・readline 履歴の最終的な解放責任

---

## リソースライフサイクル早見表

```
main 起動
  └─ mem_init          [アリーナ初期化]
  └─ env_init          [env 確保]
  └─ ms_loop
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
                      └─ pipe / fork     [シグナルハンドラ変更なし]
                           └─ (child) child_exec → execve / exit
                      └─ close(pipe fds)
                      └─ wait_all → waitpid  [子プロセス回収]
            └─ mem_pop         [アリーナ解放]
  └─ clear_history     [履歴解放]
  └─ env_free          [env 解放]
  └─ mem_reset         [アリーナ全解放]
  ※ 端末状態の保存・復元なし
```

---

## claude/ との主な差分まとめ

| 項目 | claude/ | signal-less/claude/ |
|------|---------|---------------------|
| `g_sig` グローバル変数 | 定義あり (`main.c`) | なし |
| SIGINT ハンドラ | `sig_handler_interactive` / `sig_handler_record` | デフォルト (SIG_DFL) |
| SIGQUIT ハンドラ | SIG_IGN (interactive/exec_parent/heredoc 時) | デフォルト (SIG_DFL) |
| exec 中のシグナル | 親: SIG_IGN、子: SIG_DFL | 変更なし |
| heredoc 中断検出 | `g_sig == SIGINT` → fd=-1, exit_code=130 | EOF 扱いで自然終了 |
| readline Ctrl+C | `rl_done=1` で即返り、行をフラッシュ | readline が NULL を返し loop 終了 |
| 端末 echoctl | `ms_term_disable_echoctl` で ^C 表示抑制 | なし（^C が端末に表示される） |
| `t_shell` フィールド | `term_saved`, `term_orig` あり | なし |
