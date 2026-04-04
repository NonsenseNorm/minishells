# debugged_cases

## 1. ヒアドキュメント本文中のクォートが不当に除去される

ヒアドキュメントの本文はただの文字列であり、展開するか否かはデリミタのクォート有無のみが決める。
本文中の `'` `"` はリテラル文字であり、クォーティング構文として解釈してはならない。

```bash
# bash（正しい動作）
cat <<EOF
'$HOME'
EOF
# => '/home/user'

# minishell（バグ）
cat <<EOF
'$HOME'
EOF
# => $HOME          ← クォート除去 + 展開抑制
```

原因: `heredoc_write()` がコマンドライン用の `expand_word()` を呼んでおり、
本文中のクォートがシェル構文として処理されていた。

## 2. `env_init` で `ft_strdup` 失敗時にNULLが環境変数配列に格納されSEGFAULT

`env_init` 内の `env->arr[env->len++] = ft_strdup(environ[i]);` で、
`ft_strdup` が `NULL` を返した場合、NULLが配列に格納されたまま `len` がインクリメントされる。
その後の環境変数操作（`export` 等）が `env->arr[i]` を非NULLとして参照しSEGFAULTする。

```
minishell$ export AAA=aaa
[1]    310838 segmentation fault (core dumped)  ./minishell
```

原因: `ft_strdup` の戻り値チェックが無く、malloc失敗時にNULLが有効要素として扱われていた。

## 3. heredocに100万文字級の長大な行をペーストするとフリーズする

bashでは `cat <<EOF` の本文に100万文字の行をペーストしても正常に処理されるが、
minishellでは応答不能（フリーズ）になる。

```bash
# 再現手順
python3 -c "print('A'*1000000)" | pbcopy   # 100万文字をコピー
cat <<EOF
<ペースト>
EOF
# bash: 即座に完了
# minishell: フリーズ（数分〜実質無限に待機）
```

原因: **O(n²) の文字列構築が2箇所**に存在し、n=1,000,000 で計算量が爆発する。

### ボトルネック①: `heredoc_read_canonical()` (heredoc_io.c)

非インタラクティブ入力時、1バイトずつ `read()` し毎回 `ft_strjoin()` で新文字列を生成する。

```c
// 1文字読むたびに文字列全体をmalloc+memcpy → O(n²)
while (1)
{
    r = read(STDIN_FILENO, buf, 1);       // 1バイトずつ読む
    buf[1] = 0;
    if (buf[0] == '\n')
        break ;
    tmp = ft_strjoin(line, buf);          // 既存のline全体+1文字を新領域にコピー
    free(line);
    line = tmp;
}
```

n文字の行に対し `1 + 2 + 3 + ... + n = n(n+1)/2` バイトのコピーが発生。
n=1,000,000 で約 **5×10¹¹ バイト** のメモリコピー。

#### なぜO(n²)になるか — 1文字ずつの ft_strjoin の連鎖を追う

`ft_strjoin` (libft/ft_strjoin.c) は以下の処理を行う:

```c
char *ft_strjoin(char const *s1, char const *s2)
{
    l1 = ft_strlen(s1);          // ① s1全体を走査 → O(現在の長さ)
    l2 = ft_strlen(s2);          // ② s2全体を走査 → O(1) (常に1文字)
    out = malloc(l1 + l2 + 1);   // ③ 新バッファ確保
    ft_memcpy(out, s1, l1);      // ④ 旧文字列を全コピー → O(現在の長さ)
    ft_memcpy(out + l1, s2, l2); // ⑤ 新1文字を追記 → O(1)
    return (out);
}
```

呼び出し元で `free(line)` した後 `line = tmp` するため、
ループのi回目では line の長さは i-1 文字。したがって:

| ループ回数 | lineの長さ | ft_strlen(s1) | memcpy(out,s1) | malloc | free |
|-----------|-----------|---------------|----------------|--------|------|
| 1回目     | 0         | 0回走査       | 0バイトコピー    | 2B確保  | 1B解放 |
| 2回目     | 1         | 1回走査       | 1バイトコピー    | 3B確保  | 2B解放 |
| 3回目     | 2         | 2回走査       | 2バイトコピー    | 4B確保  | 3B解放 |
| ...       | ...       | ...           | ...            | ...    | ...  |
| i回目     | i-1       | i-1回走査     | i-1バイトコピー  | i+1B確保 | iB解放 |
| n回目     | n-1       | n-1回走査     | n-1バイトコピー  | n+1B確保 | nB解放 |

**合計コスト（n文字入力時）:**
- ft_strlen の走査量: `0 + 1 + 2 + ... + (n-1) = n(n-1)/2`
- memcpy のコピー量: `0 + 1 + 2 + ... + (n-1) = n(n-1)/2`
- malloc の呼び出し回数: n回（毎回サイズが1ずつ増加）
- free の呼び出し回数: n回
- **総メモリ操作量: n(n-1) ≈ n²**

具体例（n = 100,000文字の場合）:
- ft_strlen 走査: 約 50億回のバイト比較
- memcpy コピー: 約 50億バイトの転送
- malloc/free: 各10万回（断片化によるさらなる遅延）
- → 合計 **約100億回のメモリアクセス** でフリーズ

さらに `read(STDIN_FILENO, buf, 1)` が毎文字ごとにシステムコールを発行するため、
カーネルとのコンテキストスイッチも10万回発生し、追加のオーバーヘッドとなる。

### ボトルネック②: `expand_heredoc_line()` (expand_word.c)

デリミタが非クォートの場合、本文の展開処理でも1文字ずつ `ft_substr()` + `append_str()` する。

```c
// 1文字ずつsubstr→strjoin → これもO(n²)
while (s[i])
{
    if (s[i] == '$')
        out = append_str(out, expand_var(sh, s, &i));
    else
        out = append_str(out, ft_substr(s, i++, 1));  // 1文字のsubstr + 全体join
}
```

#### なぜO(n²)になるか — append_str の連鎖を追う

`append_str` (expand_word.c:41) は `ft_strjoin` のラッパーである:

```c
static char *append_str(char *a, char *b)
{
    tmp = ft_strjoin(a, b);  // a全体 + b を新領域にコピー
    free(a);                 // 旧out を解放
    free(b);                 // ft_substr が作った1文字文字列を解放
    return (tmp);
}
```

`$` を含まない100,000文字の行では、全文字が `else` 分岐に入り:
1. `ft_substr(s, i, 1)` — 1文字分の文字列を malloc+コピーで生成（毎回2バイトのmalloc）
2. `ft_strjoin(out, 1文字)` — outの全走査 (strlen) + 全コピー (memcpy) + 新malloc
3. `free(out)` + `free(1文字文字列)` — 2回のfree

ボトルネック①と同じ構造だが、**ボトルネック①を通過した後**にこの処理が走るため、
100,000文字の行に対して**O(n²)が2回直列に実行**される。

| 処理段階 | 入力 | strlen走査量 | memcpyコピー量 | malloc回数 | free回数 |
|---------|------|-------------|---------------|-----------|---------|
| ① heredoc_read_canonical | n文字 | n²/2 | n²/2 | n | n |
| ② expand_heredoc_line | n文字 | n²/2 | n²/2 | 2n (substr+join) | 2n (old out+substr) |
| **合計** | | **n²** | **n²** | **3n** | **3n** |

n=100,000 の場合: strlen走査+memcpyで**約200億回のメモリアクセス**。

### bashとの比較: なぜbashはフリーズしないか

bashのheredoc処理は以下の点でO(n)を実現している:

1. **読み込み**: `zreadc()` / `zread()` で一度に大きなバッファ（通常8KB〜）を `read()` し、
   内部バッファからポインタ送りで1文字ずつ返す。システムコール回数は `n/8192` 回程度。
2. **行バッファ**: `string_list` / 動的配列に蓄積し、バッファが足りなくなると
   **2倍に拡張** (geometric growth) する。n文字で `realloc` は `log₂(n)` 回のみ。
3. **展開**: `param_expand()` 等も同様に動的バッファへ書き込み、
   `$` が出現しない区間はポインタ範囲で一括コピーする。

| 観点 | bash | minishell |
|------|------|-----------|
| read() 単位 | 8KB バッファ | 1バイト |
| 行の蓄積 | 幾何級数的拡張バッファ (×2) | 毎文字 malloc+memcpy (ft_strjoin) |
| 展開 | 連続リテラル区間を一括コピー | 毎文字 ft_substr+ft_strjoin |
| 計算量 | **O(n)** 償却 | **O(n²)** |
| n=10⁶ の実測 | 即座に完了 | フリーズ（実質ハング） |
