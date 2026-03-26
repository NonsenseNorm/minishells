❯ このシステムのワイルドガード部分のアルゴリズムとコードリーディングの際に読んでいくべき関数を教えて

● Explore(Explore wildcard implementation)
  ⎿  Done (20 tool uses · 36.0k tokens · 40s)
  (ctrl+o to expand)

● ワイルドカードのアルゴリズムと読むべき関数

  コアアルゴリズム（match_pattern）

  src/expander/wildcard/wildcard_utils.c:19 にある バックトラッキング型グロブマッチングです。

  パターン: "*.c"  文字列: "main.c"

  状態変数:
    star_idx  ... 最後に見た * のパターン位置
    match     ... * が対応し始めた文字列位置

  ステップ:
    1. *pat == *str → 両方進む（通常一致）
    2. *pat == '*'  → star_idx と match を保存してスキップ
    3. 不一致 かつ * あり → star_idx と match+1 に戻る（バックトラック）
    4. 不一致 かつ * なし → 失敗
    5. 文字列終端後 → 残りの * をスキップして成功

  ---
  読んでいくべき関数（呼び出し順）

  exec_cmd()                          exec_cmd/exec_cmd.c
    └─ expand_node()                  expander/expand.c:20
         ├─ expand_args()             args_expander/expand_args.c:21
         │    ├─ expand_string()      expander/expand_string.c:21   ← * を検出
         │    └─ expand_arg_wildcard()expand_args.c:62
         │         ├─ expand_wildcard()   wildcard/wildcard.c:23    ← ディレクトリ走査
         │         │    ├─ collect_matches()  wildcard.c:50         ← opendir/readdir
         │         │    │    └─ match_pattern()  wildcard_utils.c:19 ← ★コアアルゴリズム
         │         │    │         └─ match_step()  wildcard_utils.c:38
         │         │    └─ sort_str_array()  wildcard_utils.c:62    ← 結果をソート
         │         └─ insert_matches()  expand_args_utils.c:44      ← argsに展開結果を挿入
         │
         └─ expand_redirects()        redirects_expander/expand_redirects.c:19
              └─ expand_redir_wildcard()  expand_redirects.c:43    ← 複数一致で"ambiguous redirect"エラー

  ---
  ポイントまとめ

  ┌─────────────────┬────────────────────────┬─────────────────────────────────────────┐
  │      関数       │        ファイル        │                  役割                   │
  ├─────────────────┼────────────────────────┼─────────────────────────────────────────┤
  │ match_pattern   │ wildcard_utils.c:19    │ ★ 核心。* のバックトラック処理          │
  ├─────────────────┼────────────────────────┼─────────────────────────────────────────┤
  │ expand_wildcard │ wildcard.c:23          │ / を含むパスを分割してディレクトリ走査  │
  ├─────────────────┼────────────────────────┼─────────────────────────────────────────┤
  │ collect_matches │ wildcard.c:50          │ opendir/readdir で候補を列挙            │
  ├─────────────────┼────────────────────────┼─────────────────────────────────────────┤
  │ expand_string   │ expand_string.c:21     │ クォート外の * を検出してフラグを立てる │
  ├─────────────────┼────────────────────────┼─────────────────────────────────────────┤
  │ insert_matches  │ expand_args_utils.c:44 │ args配列にマッチ結果を差し込む          │
  └─────────────────┴────────────────────────┴─────────────────────────────────────────┘

  読む順番としては：
  expand_string → expand_args → expand_wildcard → collect_matches → match_pattern の順が理解しやすいです。match_pattern のバックトラック部分は match_step と合わせて読むと動作が追いやすいです。

