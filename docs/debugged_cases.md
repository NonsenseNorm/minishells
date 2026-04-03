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
