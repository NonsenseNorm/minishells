# bash/ における端末設定の変更タイミングと目的

## 概要

bash は以下の **7つの場面** で端末(TTY)の設定を変更・保存・復元する。
変更対象は大きく分けて **(A) termios フラグ** と **(B) 端末のプロセスグループ所有権** の2種類。

---

## 1. シェル起動時 — 初期状態の保存とジョブ制御の初期化

### 1-1. 端末属性の初期保存

| 項目 | 内容 |
|------|------|
| **参照** | `lib/sh/shtty.c` : `ttsave()` (L64-72) |
| **操作** | `tcgetattr(0, &ttin)` / `tcgetattr(1, &ttout)` で stdin/stdout の端末属性を保存 |
| **目的** | シェル終了時やエラー時に元の状態へ復元できるようにする |

### 1-2. ジョブ制御の初期化 — 端末の所有権取得

| 項目 | 内容 |
|------|------|
| **参照** | `jobs.c` : `initialize_job_control()` (L4740-4858) |
| **操作** | `tcgetpgrp()` で現在の端末所有者を確認し、`tcsetpgrp(shell_tty, shell_pgrp)` でシェルのプロセスグループに端末を渡す |
| **目的** | シェルがフォアグラウンドで端末を制御できるようにする。バックグラウンドから起動された場合は `SIGTTIN` を受けるまでループする |

### 1-3. ライン・ディシプリンの設定

| 項目 | 内容 |
|------|------|
| **参照** | `jobs.c` : `set_new_line_discipline()` (L4905-4940) |
| **操作** | `tcgetattr` → `c_line = NTTYDISC` → `tcsetattr` |
| **目的** | 古いシステムで新しいライン・ディシプリン (NTTYDISC) に切り替える（現代のシステムではほぼ不要） |

---

## 2. readline 入力時 — raw モードへの切替と復元

### 2-1. 入力開始: 端末を readline 用に準備

| 項目 | 内容 |
|------|------|
| **参照** | `lib/readline/rltty.c` : `rl_prep_terminal()` (L607-687) → `prepare_terminal_settings()` (L514-590) |
| **操作** | `tcgetattr` で現在の設定を `otio` に保存した後、以下を変更して `tcsetattr` で適用: |
| **変更フラグ** | `c_lflag`: `-ICANON`, `-ECHO` (raw モード化) / `c_iflag`: `-IXON`, `-IXANY` (フロー制御無効) / `c_cc[VMIN]=1, VTIME=0` (1文字ずつ即座に返す) / ECHOCTL の値を `_rl_echoctl` に保存 |
| **目的** | readline が1文字ずつキー入力を処理し、行編集・補完・履歴検索を実現する |

### 2-2. 入力完了: 元の端末設定を復元

| 項目 | 内容 |
|------|------|
| **参照** | `lib/readline/rltty.c` : `rl_deprep_terminal()` (L690-729) |
| **操作** | 保存しておいた `otio` を `tcsetattr` で書き戻す |
| **目的** | コマンド実行中は通常の canonical モードに戻す。外部コマンドが正しく端末を使えるようにする |

---

## 3. ジョブ制御 — フォアグラウンドジョブの切替

### 3-1. フォアグラウンドジョブ開始: 端末を渡す

| 項目 | 内容 |
|------|------|
| **参照** | `jobs.c` : `give_terminal_to()` (L5000-5033) |
| **操作** | `tcsetpgrp(shell_tty, job_pgrp)` で端末の所有権をジョブのプロセスグループに移す |
| **目的** | 実行中のフォアグラウンドプロセスが端末から入力を読めるようにする |

### 3-2. フォアグラウンドジョブ終了/停止: 端末を回収して設定を復元

| 項目 | 内容 |
|------|------|
| **参照** | `jobs.c` : `wait_for()` 内 (L3253, L3280, L3301, L3913-3940) |
| **操作** | `give_terminal_to(shell_pgrp, 0)` で端末をシェルに戻す → `get_tty_state()` / `set_tty_state()` で端末属性を保存・復元 |
| **目的** | ジョブ（例: vim, less）が端末設定を変更した可能性があるため、シェルの端末設定を復元する |

### 3-3. get_tty_state / set_tty_state

| 項目 | 内容 |
|------|------|
| **参照** | `jobs.c` : `get_tty_state()` (L2618-2650), `set_tty_state()` (L2655-2685) |
| **操作** | `tcgetattr(tty, &shell_tty_info)` / `tcsetattr(tty, TCSADRAIN, &shell_tty_info)` |
| **目的** | シェル用の端末設定 (`shell_tty_info`) を一元管理する。フォアグラウンドジョブの前後で呼ばれる |

---

## 4. `read` ビルトイン — 特殊な入力モード

### 4-1. read -s (サイレントモード)

| 項目 | 内容 |
|------|------|
| **参照** | `builtins/read.def` (L587-620) |
| **操作** | `ttgetattr(fd, &ttattrs)` で保存 → `ttfd_cbreak(fd, &ttset)` で cbreak モード (ICANON 無効, ECHO 無効) に設定 |
| **目的** | パスワード入力のようにエコーを無効にしつつ1文字ずつ読む |

### 4-2. read -e (readline 使用) 以外のエコー無効

| 項目 | 内容 |
|------|------|
| **参照** | `builtins/read.def` (L608-617) |
| **操作** | `ttfd_noecho(fd, &ttset)` で ECHO, ECHOK, ECHONL を無効化 |
| **目的** | エコーを無効にしつつ canonical モードは維持する |

### 4-3. read 終了時の復元

| 項目 | 内容 |
|------|------|
| **参照** | `builtins/read.def` : `ttyrestore()` (L1226-1231), `read_tty_cleanup()` (L1239-1244) |
| **操作** | `ttsetattr(fd, &saved_attrs)` で保存した設定を復元 |
| **目的** | read コマンド終了後（正常終了・シグナルによる中断いずれでも）端末を元に戻す |

---

## 5. シグナル処理 — 端末の緊急復元と再準備

### 5-1. シグナル受信後のクリーンアップ

| 項目 | 内容 |
|------|------|
| **参照** | `lib/readline/signals.c` : `rl_cleanup_after_signal()` (L574-582) |
| **操作** | `rl_deprep_term_function()` で端末を復元 |
| **目的** | SIGINT 等を受けた際に raw モードのまま放置されるのを防ぐ |

### 5-2. シグナルハンドラ復帰後の再準備

| 項目 | 内容 |
|------|------|
| **参照** | `lib/readline/signals.c` : `rl_reset_after_signal()` (L585-591) |
| **操作** | `rl_prep_term_function()` で端末を再度 raw モードに設定 |
| **目的** | シグナル処理後に readline の入力を再開する |

### 5-3. シグナル生成の一時無効化

| 項目 | 内容 |
|------|------|
| **参照** | `lib/readline/rltty.c` : `_rl_disable_tty_signals()` (L968-988), `_rl_restore_tty_signals()` (L990-1003) |
| **操作** | `c_lflag &= ~ISIG` / `c_iflag &= ~IXON` → `tcsetattr` |
| **目的** | 特定のタイミングでキー入力がシグナルを生成しないようにする（例: Ctrl+C をリテラル文字として読む） |

---

## 6. fc コマンド（edit-and-execute）— エディタ起動時の端末復元

| 項目 | 内容 |
|------|------|
| **参照** | `bashline.c` : `edit_and_execute_command()` (L983-995) |
| **操作** | `rl_deprep_term_function()` → エディタ実行 → `rl_prep_term_function()` |
| **目的** | 外部エディタ（vim 等）が canonical モードの端末を必要とするため、readline の raw モードを一時解除する |

---

## 7. シェル終了時 — 最終クリーンアップ

| 項目 | 内容 |
|------|------|
| **参照** | `shell.c` : `exit_shell()` (L985-996) |
| **操作** | `rl_deprep_term_function()` で readline の端末設定を復元 + `read_tty_cleanup()` で read ビルトインの設定を復元 |
| **目的** | シェル終了後に親プロセス（ターミナルエミュレータ等）に正常な端末状態を返す |

---

## 8. ウィンドウサイズ — SIGWINCH 対応

| 項目 | 内容 |
|------|------|
| **参照** | `lib/sh/winsize.c` : `get_new_window_size()` (L90-111), `lib/readline/terminal.c` : `_rl_get_screen_size()` (L292-340) |
| **操作** | `ioctl(fd, TIOCGWINSZ, &win)` でウィンドウサイズを取得 |
| **目的** | ターミナルのリサイズに追従して LINES/COLUMNS を更新し、readline の表示を正しく保つ |

---

## フロー概要

```
シェル起動
  |
  +-- [1] ttsave(): 初期端末属性を保存
  +-- [1] initialize_job_control(): tcsetpgrp() で端末を取得
  |
  v
メインループ (read_command → execute_command)
  |
  +-- [2] rl_prep_terminal(): raw モードに設定
  |     readline が行編集を処理
  +-- [2] rl_deprep_terminal(): canonical モードに復元
  |
  +-- コマンド実行
  |     +-- [3] give_terminal_to(job_pgrp): 端末をジョブに渡す
  |     +-- ジョブ実行中...
  |     +-- [3] give_terminal_to(shell_pgrp): 端末をシェルに回収
  |     +-- [3] get_tty_state() / set_tty_state(): 端末属性を復元
  |     |
  |     +-- (read ビルトインの場合)
  |     |     +-- [4] ttfd_cbreak/noecho: 特殊モード設定
  |     |     +-- [4] ttyrestore(): 元に戻す
  |     |
  |     +-- (fc コマンドの場合)
  |           +-- [6] rl_deprep → エディタ → rl_prep
  |
  +-- (シグナル受信時)
  |     +-- [5] rl_cleanup_after_signal(): 端末復元
  |     +-- [5] rl_reset_after_signal(): 端末再準備
  |
  +-- (SIGWINCH 受信時)
        +-- [8] ioctl(TIOCGWINSZ): ウィンドウサイズ更新
  |
  v
シェル終了
  +-- [7] exit_shell(): rl_deprep + read_tty_cleanup
```
