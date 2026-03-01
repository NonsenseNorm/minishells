#include "lexer.h"

/*
** Advance *i past the end of the current word token in `line`.
**
** A word ends at whitespace, an operator character (| < >), or '\0'.
** Inside a single-quoted ('...') or double-quoted ("...") region,
** all characters are part of the word until the matching closing quote.
**
** Returns 0 if the word ended cleanly.
** Returns the unclosed quote character ('\'' or '"') if '\0' was reached
** while still inside a quoted region.
*/
int	lex_word_end(const char *line, int *i)
{
	(void)line;
	(void)i;
	return (0);
}
