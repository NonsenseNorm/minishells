#include "types.h"

/* ================================================================
   実装済みユーティリティ関数 (変更不要)
   ================================================================ */

int	is_redirect(t_tok_type t)
{
	return (t == TOK_REDIRECT_IN || t == TOK_REDIRECT_OUT
		|| t == TOK_REDIRECT_APPEND || t == TOK_HEREDOC);
}

t_redirect_type	map_redirect(t_tok_type t)
{
	if (t == TOK_REDIRECT_IN)
		return (REDIRECT_IN);
	if (t == TOK_REDIRECT_OUT)
		return (REDIRECT_OUT);
	if (t == TOK_REDIRECT_APPEND)
		return (REDIRECT_APPEND);
	return (REDIRECT_HEREDOC);
}

void	free_pipeline(t_pipeline *pl)
{
	int			i;
	t_redirect	*r;
	t_redirect	*next;

	if (!pl->cmds)
		return ;
	i = 0;
	while (i < pl->count)
	{
		free(pl->cmds[i].argv);
		r = pl->cmds[i].redirects;
		while (r)
		{
			next = r->next;
			free(r);
			r = next;
		}
		i++;
	}
	free(pl->cmds);
	pl->cmds = NULL;
	pl->count = 0;
}

/* ================================================================
   エラーヘルパー (実装済み)
   ================================================================ */

static int	syntax_err(t_token *tok)
{
	const char	*repr;

	repr = "newline";
	if (tok && tok->type == TOK_PIPE)
		repr = "|";
	fprintf(stderr,
		"minishell: syntax error near unexpected token `%s'\n", repr);
	return (1);
}

/*
** add_redirect -- リダイレクト構造体を作って cmd->redirects リストの末尾に追加する。
** (parse_one の第2パスで使う)
*/
static int	add_redirect(t_cmd *cmd, t_token *tok)
{
	t_redirect	*r;
	t_redirect	*cur;

	r = malloc(sizeof(*r));
	if (!r)
		return (1);
	r->type = map_redirect(tok->type);
	r->target = tok->next->value;
	r->next = NULL;
	if (!cmd->redirects)
		cmd->redirects = r;
	else
	{
		cur = cmd->redirects;
		while (cur->next)
			cur = cur->next;
		cur->next = r;
	}
	return (0);
}

/* ================================================================
   TODO: 以下の関数を実装してください。
   実装の手順は PARSER_GUIDE.md を参照してください。
   ================================================================ */

/*
** count_cmds -- トークンリストのパイプ数 + 1 を返す。
**   [WORD][PIPE][WORD][PIPE][WORD] → 3
**
** Step 1 で実装する。
*/
static int	count_cmds(t_token *tok)
{
	int	count;

	count = 0;
	while (tok)
	{
		if (tok->type == TOK_PIPE)
			++count;
		tok = tok->next;
	}
	return (count); /* TODO: Step 1 */
}

/*
** parse_one -- トークン列から1コマンド分を解析して cmd を埋める。
**
** *tokp は解析開始位置を指す (TOK_PIPE か NULL になるまで進む)。
** 終了後 *tokp は次の TOK_PIPE (または NULL) を指す。
**
** 2パス構成:
**   第1パス: TOK_WORD の数を数えて argc を求め、argv を malloc する
**   第2パス: argv と redirects を埋める (add_redirect を使う)
**
** Returns 0 on success, 1 on syntax error.
**
** Step 3 で実装する。
*/
static int	parse_one(t_token **tokp, t_cmd *cmd)
{
	t_token	*tok;
	int		argc;
	int		k;

	tok = tokp;
}

/*
** parse_cmds -- pl->count 個のコマンドを順に parse_one で解析する。
** コマンド間の TOK_PIPE をスキップし、余剰トークンがあればエラー。
** (この関数は完成形。parse_one を実装すると自動的に動く)
*/
static int	parse_cmds(t_token *tok, t_pipeline *pl)
{
	int	i;

	i = 0;
	while (i < pl->count)
	{
		if (parse_one(&tok, &pl->cmds[i]) != 0)
			return (1);
		if (tok && tok->type == TOK_PIPE)
			tok = tok->next;
		i++;
	}
	if (tok)
		return (syntax_err(tok));
	return (0);
}

/*
** parse_pipeline -- パーサーのエントリポイント。
**
** 呼び出し構造 (スキャフォールド済み):
**   count_cmds → malloc → parse_cmds → parse_one
**
** Step 1: count_cmds を実装する
** Step 2: 先頭の NULL / TOK_PIPE チェックを追加する (syntax_err)
** Step 3: parse_one を実装する
*/
int	parse_pipeline(t_token *tok, t_pipeline *pl)
{
	pl->cmds = NULL;
	pl->count = 0;
	/* TODO: Step 2 — 先頭が NULL または TOK_PIPE なら syntax_err(tok) を返す */
	if (tok == 0x00 || tok->type == TOK_PIPE)
		return (syntax_err(tok));
	pl->count = count_cmds(tok); /* Step 1 未実装だと 0 のまま */
	if (pl->count == 0)
		return (1);
	pl->cmds = malloc(sizeof(t_cmd) * (size_t)pl->count);
	if (!pl->cmds)
		return (1);
	if (parse_cmds(tok, pl) != 0)
	{
		free_pipeline(pl);
		return (1);
	}
	return (0);
}

ふせいこうぶんのにゅうりょくによわいとおもう
