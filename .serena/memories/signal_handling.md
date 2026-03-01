# Signal Handling for minishell (signal-less/claude)

## Working implementation: /Users/0/minishell/signal-less/claude/

## Key bugs found and fixed

### 1. macOS signal() uses SA_RESTART
- `signal()` on macOS sets SA_RESTART by default
- `read()` in heredoc child restarts instead of returning EINTR when SIGINT fires
- Fix: use `sigaction` with `sa_flags = 0` (no SA_RESTART) in `sig_set_heredoc()`

### 2. g_sig checked AFTER sig_set_interactive() resets it
- `sig_set_interactive()` always sets `g_sig = 0`
- The `if (g_sig == SIGINT)` check after it was always false
- Fix: check `g_sig == SIGINT` BEFORE calling `sig_set_interactive()`

### 3. rl_redisplay() doesn't output prompt bytes in PTY context
- GNU readline 8.3 `rl_redisplay()` emits `\x1b[?2004h` but no prompt text
- Fix: manually write `rl_prompt` then call `rl_on_new_line_with_prompt()`

### 4. flush_sigint for rl_redisplay approach
- With `rl_redisplay()`, readline never returns on SIGINT (stays blocking)
- NULL+SIGINT means Ctrl+D after Ctrl+C → should break, not continue
- flush_sigint: just set exit_code=130 and g_sig=0 (no return value)

## signal.c final pattern
```c
static void sig_handler_interactive(int sig)
{
    g_sig = sig;
    write(STDOUT_FILENO, "\n", 1);
    rl_replace_line("", 0);
    if (rl_prompt)
        write(STDOUT_FILENO, rl_prompt, ft_strlen(rl_prompt));
    rl_on_new_line_with_prompt();
}

void sig_set_heredoc(void)
{
    struct sigaction sa;
    g_sig = 0;
    signal(SIGQUIT, SIG_IGN);
    sa.sa_handler = sig_handler_record;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;  // NO SA_RESTART
    sigaction(SIGINT, &sa, NULL);
}
```

## heredoc.c g_sig check order
```c
// WRONG:
close(p[1]);
sig_set_interactive();    // resets g_sig=0!
if (g_sig == SIGINT)      // always false!

// CORRECT:
close(p[1]);
if (g_sig == SIGINT)      // check before reset
    return (sig_set_interactive(), close(p[0]), sh->exit_code = 130, -1);
sig_set_interactive();
```

## Test script
Use ISIG-aware PTY (don't call tty.setraw on slave). Must set TIOCSCTTY.
Tests at /tmp/ (test_signals2.py has a timeout bug; use inline script instead).
