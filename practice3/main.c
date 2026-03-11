#include "types.h"

int	main(void)
{
	char		*line;
	t_token		*tok;
	t_pipeline	pl;
	int			status;

	while (1)
	{
		line = readline("exec> ");
		if (!line)
		{
			printf("\n");
			break ;
		}
		if (!*line)
		{
			free(line);
			continue ;
		}
		add_history(line);
		tok = NULL;
		if (lex_line(line, &tok) == 0)
		{
			pl.cmds = NULL;
			pl.count = 0;
			if (parse_pipeline(tok, &pl) == 0)
			{
				status = exec_pipeline(&pl);
				printf("[exit: %d]\n", status);
				free_pipeline(&pl);
			}
		}
		free_tokens(tok);
		free(line);
	}
	return (0);
}
