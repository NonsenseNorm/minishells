# Minishell Project Overview

## Purpose
42 school minishell project — a bash-like shell implemented in C.

## Project Layout
```
/Users/0/minishell/
├── claude/          ← our implementation (primary)
│   ├── Makefile
│   ├── libft/       ← custom libft (subset, no lst/put/strmapi)
│   └── src/
│       ├── core/    ms.h (main header), main.c, loop.c, error.c
│       ├── signal/  signal.c
│       ├── mem/     mem.c (arena allocator + str helpers)
│       ├── env/     env.c (init/free/grow), env_getset.c (get/set/unset)
│       ├── lexer/   lexer.c, lexer_word.c
│       ├── parser/  parser.c, parser_cmd.c, parser_util.c
│       ├── expand/  expand.c, expand_pipeline.c
│       ├── exec/    exec.c, pipeline.c, heredoc.c, redirect.c, path.c
│       └── builtin/ builtin.c, cd.c, echo_pwd_env.c, export_unset.c, exit.c
├── monbuse-minishell/42-minishell/  ← reference impl (use for signal)
├── codex/           ← another reference (do NOT use for signal)
├── bash/            ← bash source reference
└── dash/            ← dash source reference
```

## Tech Stack
- Language: C (C99 / POSIX)
- Build: make
- Key libs: readline (homebrew on macOS), libft (custom)

## Build Commands
```bash
cd claude && make          # build minishell
cd claude && make re       # rebuild from scratch
cd claude && make fclean   # clean all artifacts
```

## macOS Notes
- Use homebrew readline: Makefile sets `-I$(brew --prefix readline)/include`
- Include order in ms.h: `stdio.h` and `stdlib.h` MUST come before readline headers

## Signal Architecture
- `g_sig` (volatile sig_atomic_t) tracks last signal, declared in main.c, extern in ms.h
- `sig_set_interactive()` — SIGQUIT ignored, SIGINT prints newline + refreshes prompt; resets g_sig=0
- `sig_set_exec_parent()` — both SIG_IGN (children get signal via process group)
- `sig_set_exec_child()` — both SIG_DFL (child can be killed)
- `sig_set_heredoc()` — SIGQUIT ignored, SIGINT records in g_sig; resets g_sig=0
- Reference: monbuse-minishell/42-minishell/signal.c (NOT codex)

## Global State
- Only one global allowed by norm: `volatile sig_atomic_t g_sig`
