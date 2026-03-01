#include "../core/ms.h"

static void	disable_echoctl_now(void)
{
	struct termios	term;

	if (tcgetattr(STDIN_FILENO, &term) != 0)
		return ;
#ifdef ECHOCTL
	term.c_lflag &= ~ECHOCTL;
#endif
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

static void	sigint_prompt(int sig)
{
	(void)sig;
	g_sig = SIGINT;
	disable_echoctl_now();
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_redisplay();
}

static void	sigint_heredoc(int sig)
{
	(void)sig;
	g_sig = SIGINT;
	disable_echoctl_now();
	write(STDOUT_FILENO, "\n", 1);
}

void	sig_set_interactive(void)
{
	g_sig = 0;
	signal(SIGINT, sigint_prompt);
	signal(SIGQUIT, SIG_IGN);
}

void	sig_set_exec_parent(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void	sig_set_exec_child(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

void	sig_set_heredoc(void)
{
	g_sig = 0;
	signal(SIGINT, sigint_heredoc);
	signal(SIGQUIT, SIG_IGN);
}
