# 字句解析器 (Lexer) 実装ガイド

## レキサーとは

シェルがコマンド `echo "hello world" | cat > out` を受け取ったとき、
まず「単語と記号の列」に分解する必要がある。これが字句解析 (lexing) だ。

```
入力文字列:   echo "hello world" | cat > out
              ↓
トークン列:   [WORD "echo"] → [WORD "\"hello world\""] → [PIPE] → [WORD "cat"] → [REDIR_OUT] → [WORD "out"]
```

トークンには2種類ある:

| 種類 | 内容 | value |
|---|---|---|
| WORD | 単語 (クォートも含む生文字列) | 文字列へのポインタ |
| 演算子 | `\|` `<` `>` `>>` `<<` | NULL |

> **なぜ value にクォートを含む生文字列を入れるのか?**
> クォートの除去と変数展開は次の「展開フェーズ」で行う。
> レキサーはあくまで「どこで区切るか」だけに集中する。

---

## 関数の役割分担

```
lex_line(line, &out)
 │
 ├─ 空白をスキップ
 ├─ 次の文字が | < > なら → lex_op でオペレータトークンを追加
 └─ それ以外 → lex_word_end でワードの終端を探し、ワードトークンを追加
                  │
                  └─ クォート内の | < > はワードの一部として読み飛ばす
```

`lex_word_end` は「ここからワードがどこまで続くか」だけを教えるヘルパー。
トークンの生成は `lex_line` が行う。

---

## 実装の進め方 (5ステップ)

### Step 1 — `lex_word_end`: クォートなし版

**ゴール**: `*i` をワードの末尾まで進める。ただしクォートは無視する。

ワードが終わる条件:
- 文字列の終端 `'\0'`
- 空白文字 (space, tab など)
- オペレータ文字 `| < >`

例:
```
line = "echo|cat",  *i = 0  →  終了後 *i = 4   ("|" の直前)
line = "echo hello", *i = 0  →  終了後 *i = 4   (" " の直前)
line = "echo",       *i = 0  →  終了後 *i = 4   ('\0' の直前)
```

**この時点では `lex_line` がまだスタブなのでテストは通らない。**
Step 3 で `lex_line` を実装した後に効果が出る。

---

### Step 2 — `lex_line`: オペレータのトークン化

**ゴール**: `|`, `<`, `>`, `>>`, `<<` を認識してトークンを追加する。ワードはまだ無視してよい。

通るようになるテスト (現在0→これらがPASSになる):
```
pipe           "|"
redir in       "<"
redir out      ">"
redir append   ">>"
heredoc        "<<"
```

実装方針:
```
while (line[i] != '\0')
  空白ならスキップ
  | か < か > なら:
    2文字か1文字かを判定して適切な型のトークンを push
    i を進める
  それ以外はとりあえず i++ で読み飛ばす (ワードは Step 3 で対応)
```

`new_tok` と `push_tok` はすでに実装済みで使える。
演算子トークンの value は NULL でよい。

**判定の注意**: `>>` と `>` を見分けるには `line[i+1]` を見ること。
`>` の前に `>>` を確認しないと `>>` が `>` `>` に分解されてしまう。

---

### Step 3 — `lex_line`: ワードのトークン化

**ゴール**: Step 1 の `lex_word_end` を使ってワードトークンを追加する。

新たに通るようになるテスト:
```
single word           "echo"
two words             "echo hello"
extra spaces          "  echo  hi  "
cmd | cmd             "echo | cat"
cmd > file            "echo > out"
(その他オペレータ混在系すべて)
pipe no space         "echo|cat"
redir in no space     "cat<in"
redir append no space "echo>>out"
```

実装方針:
```
while (line[i] != '\0')
  空白ならスキップ
  | か < か > なら: (Step 2 と同じ)
    オペレータトークンを push
  それ以外 (ワードの開始):
    st = i  ← ワードの開始位置を記録
    lex_word_end(line, &i)  ← i をワードの末尾まで進める
    line[st..i) の部分文字列を malloc してトークンに入れる
    ワードトークンを push
```

部分文字列の切り出しには `strndup(line + st, i - st)` が使える。
value はこの malloc した文字列へのポインタになる。

---

### Step 4 — `lex_word_end`: クォート対応

**ゴール**: クォート `'...'` `"..."` の内側では空白もオペレータも
ワードの一部として読み飛ばす。

新たに通るようになるテスト:
```
single quoted       "echo 'hello world'"
empty single quotes "''"
quote glued to word "echo 'hi'world"
pipe inside quotes  "'hello|world'"
double quoted       "echo \"hello world\""
empty double quotes "\"\""
dquote with pipe    "\"a|b\" | cat"
```

クォートの状態を変数 `q` (現在開いているクォート文字、なければ `0`) で管理する:

```
q = 0
while (line[*i] != '\0')
  if (q == 0 かつ 空白またはオペレータ)
    break  ← ワード終了
  if (q == 0 かつ line[*i] が ' か ")
    q = line[*i]  ← クォート開始
  else if (q != 0 かつ line[*i] == q)
    q = 0  ← クォート終了
  (*i)++
```

ループを抜けた後:
- `q == 0` ならクォートは正常に閉じられた → return 0
- `q != 0` ならクォートが未閉じ → return q (後でエラー判定に使う)

---

### Step 5 — エラー処理

**ゴール**: 未閉じクォートを検出して `lex_line` からエラーを返す。

通るようになるテスト:
```
unclosed single quote   "echo 'hello"   → return 1
unclosed double quote   "echo \"hello"  → return 1
```

`lex_word_end` が非ゼロ (未閉じクォート文字) を返したとき、
`lex_line` はそのままエラーメッセージを表示して 1 を返す。

エラー時に表示するメッセージの例 (bash に合わせる場合):
```
minishell: unexpected EOF while looking for matching `''
minishell: syntax error: unexpected end of file
```

---

## テストの実行方法

```bash
make test          # テストランナーをビルドして実行
make               # インタラクティブ版をビルド
./lexer            # 実際に動かして試す
```

## 実装完了の目安

```
29/29 tests passed
```

---

## 補足: トークンのメモリ管理

`lex_line` が生成したトークンリストは呼び出し側 (`main.c`) が `free_tokens` で解放する。
よって:
- `new_tok` に渡す value は必ず `malloc` した文字列であること
- 演算子トークンの value は NULL (解放されない)
- `lex_line` が途中でエラーになった場合も、それまでに作ったトークンは
  `*out` に繋いでおけば呼び出し側が解放できる
