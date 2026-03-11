#include "types.h"

/* ================================================================
   ヒアドック中断フラグ (Step 4 で使用)
   ================================================================ */

static volatile int	g_heredoc_interrupted = 0;

/* ================================================================
   TODO: 以下の関数を実装してください。
   実装の手順は HEREDOC_GUIDE.md を参照してください。
   ================================================================ */

/*
** read_heredoc -- delimiter が入力されるまで readline でコンテンツを読み込む。
**
** 処理:
**   1. readline("> ") でユーザーの入力を1行ずつ取得する
**   2. 入力が delimiter と一致したら読み込み終了
**   3. 一致しない場合は content に "line\n" を追記する
**   4. readline が NULL を返した場合 (Ctrl-D / SIGINT) はループを抜ける
**
** 戻り値: malloc した文字列 (ヒアドックのコンテンツ全体)
**
** Step 1 で実装する。
*/
char	*read_heredoc(const char *delimiter)
{
	char	*line;
	char	*content;
	char	*tmp;

	content = strdup("");
	if (!content)
		return (NULL);
	while (!g_heredoc_interrupted)
	{
		line = readline("> ");
		if (!line)
			break ; /* EOF または SIGINT で中断 */
		/* TODO: Step 1
		**   strcmp(line, delimiter) == 0 なら free(line) して break
		**   そうでなければ content に line と "\n" を追記する
		**   追記には strcat か 手動の realloc + strcpy を使う
		*/
		(void)delimiter;
		(void)tmp;
		free(line);
	}
	return (content); /* TODO: Step 1 — 正しく構築した content を返す */
}

/*
** write_heredoc_to_pipe -- ヒアドックのコンテンツをパイプに書き込む。
**
** 処理:
**   1. read_heredoc(r->target) でコンテンツを取得
**   2. pipe(pipe_fd) でパイプを作成
**   3. write(pipe_fd[1], content, strlen(content)) で書き込む
**   4. close(pipe_fd[1]) で書き込み端を閉じる
**   5. r->hd_fd = pipe_fd[0] に読み取り端を保存
**
** 戻り値: 0 (成功), 1 (失敗)
**
** Step 2 で実装する。
*/
static int	write_heredoc_to_pipe(t_redirect *r)
{
	char	*content;
	int		pipe_fd[2];

	content = read_heredoc(r->target); /* Step 1 が必要 */
	if (!content)
		return (1);
	/* TODO: Step 2
	**   pipe(pipe_fd) を呼ぶ (失敗したら free(content) して return 1)
	**   write(pipe_fd[1], content, strlen(content))
	**   close(pipe_fd[1])
	**   r->hd_fd = pipe_fd[0]
	*/
	(void)pipe_fd;
	free(content);
	return (0); /* TODO: Step 2 — 成功したら 0 を返す */
}

/*
** prepare_pipeline_heredocs -- パイプライン全体のヒアドックを準備する。
**
** 処理:
**   パイプライン内の全コマンドのリダイレクトを走査し、
**   type == REDIRECT_HEREDOC のものに write_heredoc_to_pipe を呼ぶ。
**
** 戻り値: 0 (全成功), 1 (いずれかが失敗 または SIGINT 中断)
**
** Step 3 で実装する。
*/
int	prepare_pipeline_heredocs(t_pipeline *pl)
{
	/* Step 3 実装前のコンパイルエラー防止 (実装後に削除してよい) */
	if (0)
		write_heredoc_to_pipe(NULL);
	/* TODO: Step 3
	**   i = 0; while (i < pl->count) でコマンドを走査
	**   r = pl->cmds[i].redirects; while (r) でリダイレクトを走査
	**   r->type == REDIRECT_HEREDOC なら write_heredoc_to_pipe(r) を呼ぶ
	**   失敗したら 1 を返す
	*/
	(void)pl;
	return (0); /* TODO: Step 3 */
}
