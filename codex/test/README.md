# Test Selection Notes

This test suite references GNU Bash upstream tests, but only covers features currently implemented in this minishell.

Selected source groups:

- `bash/tests/errors.tests`
  - Syntax errors (`|`, redirection target errors, unclosed quote behavior)
  - Command execution errors (`command not found`, missing path, permission denied)
  - `exit` builtin error handling
- `bash/tests/redir.tests`
  - Basic `<`, `>`, `>>` redirections
- `bash/tests/heredoc.tests`
  - Quoted vs unquoted heredoc delimiter expansion behavior
- `bash/tests/misc/sigint-*.sh`
  - Foreground command signal exit-status behavior (`SIGINT`, `SIGQUIT`)
- `bash/tests/dollar*` and quote-related tests
  - `$?` expansion and quote-sensitive expansion paths

Explicitly out of scope in this suite:

- Job control, `trap`, arithmetic, arrays, globstar/extglob, process substitution, coproc, and other features not implemented in this minishell.
