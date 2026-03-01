#include "ms.h"

volatile sig_atomic_t	g_sig;

void	ms_term_disable_echoctl(t_shell *sh)
{
	struct termios	term;

	if (!sh->interactive)
		return ;
	if (tcgetattr(STDIN_FILENO, &term) != 0)
		return ;
#ifdef ECHOCTL
	term.c_lflag &= ~ECHOCTL;
#endif
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

static void	restore_terminal(t_shell *sh)
{
	if (!sh->interactive || !sh->term_saved)
		return ;
	tcsetattr(STDIN_FILENO, TCSANOW, &sh->term_orig);
}

static int	init_shell(t_shell *sh, char **envp)
{
	sh->exit_code = 0;
	sh->interactive = isatty(STDIN_FILENO);
	sh->term_saved = false;
	if (env_init(&sh->env, envp) != 0)
		return (1);
	mem_init(&sh->mem);
	if (!sh->mem.top)
	{
		env_free(&sh->env);
		return (1);
	}
	if (sh->interactive)
	{
		if (tcgetattr(STDIN_FILENO, &sh->term_orig) == 0)
			sh->term_saved = true;
		ms_term_disable_echoctl(sh);
	}
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	sh;

	(void)argc;
	(void)argv;
	if (init_shell(&sh, envp) != 0)
		return (1);
	ms_loop(&sh);
	restore_terminal(&sh);
	clear_history();
	env_free(&sh.env);
	mem_reset(&sh.mem);
	return (sh.exit_code);
}
