#include "../core/ms.h"

static int	open_out(char *path, int app)
{
	if (app)
		return (open(path, O_WRONLY | O_CREAT | O_APPEND, 0644));
	return (open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644));
}

int	apply_redirects(t_shell *sh, t_redirect *r)
{
	int	fd;

	(void)sh;
	while (r)
	{
		fd = -1;
		if (r->type == REDIRECT_IN)
			fd = open(r->target, O_RDONLY);
		else if (r->type == REDIRECT_OUT)
			fd = open_out(r->target, 0);
		else if (r->type == REDIRECT_APPEND)
			fd = open_out(r->target, 1);
		else if (r->type == REDIRECT_HEREDOC)
			fd = heredoc_fd(sh, r->target, r->quoted);
		if (fd < 0)
		{
			if (r->type == REDIRECT_HEREDOC && sh->exit_code == 130)
				return (130);
			return (ms_perror(r->target, NULL, 1));
		}
		if (r->type == REDIRECT_IN || r->type == REDIRECT_HEREDOC)
			dup2(fd, STDIN_FILENO);
		else
			dup2(fd, STDOUT_FILENO);
		close(fd);
		r = r->next;
	}
	return (0);
}
