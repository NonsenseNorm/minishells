/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_redirects.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 16:07:51 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/21 23:53:59 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/minishell.h"

static int	expand_redir_wildcard(t_redirect *redir, char *expanded);
static void	handle_empty_redir(t_redirect *redir, char *expanded);
static void	redir_ambiguous_error(t_redirect *redir);

void	expand_redirects(t_node *node, t_env *env_list)
{
	t_redirect	*redir;
	char		*expanded;
	int			has_wildcard;

	redir = node->redirects;
	while (redir)
	{
		if (redir->type != TK_HEREDOC)
		{
			expanded = expand_string(
					redir->filename, env_list, &has_wildcard);
			if (has_wildcard
				&& expand_redir_wildcard(redir, expanded))
				return ;
			if (!has_wildcard)
				handle_empty_redir(redir, expanded);
		}
		redir = redir->next;
	}
}

// expand redir wildcard. if ambiguous return 1
static int	expand_redir_wildcard(t_redirect *redir, char *expanded)
{
	char	**matches;
	char	*new_filename;
	int		count;

	matches = expand_wildcard(expanded);
	count = 0;
	while (matches && matches[count])
		count++;
	if (count == 1)
	{
		new_filename = ft_strdup(matches[0]);
		free_str_array(matches);
		free(expanded);
		if (!new_filename)
			return (1);
		free(redir->filename);
		redir->filename = new_filename;
		return (0);
	}
	free_str_array(matches);
	free(expanded);
	if (count == 0)
		return (0);
	redir_ambiguous_error(redir);
	return (1);
}

static void	redir_ambiguous_error(t_redirect *redir)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(redir->filename, STDERR_FILENO);
	ft_putstr_fd(": ambiguous redirect\n", STDERR_FILENO);
	redir->filename[0] = '\0';
}

// ambiguous redirect
static void	handle_empty_redir(t_redirect *redir, char *expanded)
{
	if (!expanded[0] && has_unquoted_dollar(redir->filename))
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(redir->filename, STDERR_FILENO);
		ft_putstr_fd(": ambiguous redirect\n", STDERR_FILENO);
		free(expanded);
		redir->filename[0] = '\0';
		return ;
	}
	free(redir->filename);
	redir->filename = expanded;
}
