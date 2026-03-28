*This project has been created as part of the 42 curriculum by stanizak, skawakat*

# minishell

## Description

minishell is a simplified shell implementation written in C, inspired by Bash. The goal of this project is to understand how a Unix shell works under the hood by building one from scratch: tokenizing input, parsing command structures, expanding variables, managing processes, and handling signals.

The shell supports interactive command-line input via GNU Readline, pipes, redirections, here-documents, environment variable expansion, and a set of built-in commands.

### Features

- **Interactive prompt** with command history (GNU Readline)
- **Pipes** (`|`) to chain commands
- **Redirections**: input (`<`), output (`>`), append (`>>`), here-document (`<<`)
- **Environment variable expansion** (`$VAR`, `$?`)
- **Quoting**: single quotes (literal), double quotes (with expansion)
- **Built-in commands**: `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`
- **Signal handling**: `Ctrl+C`, `Ctrl+D`, `Ctrl+\` behave like Bash
- **Custom arena memory allocator** for efficient per-command allocation

## Instructions

### Requirements

- A C compiler (`cc`, `gcc`, or `clang`)
- GNU Readline library (`libreadline-dev` on Debian/Ubuntu, or `readline` via Homebrew on macOS)
- `make`

### Compilation

```bash
make        # Build the minishell executable
make clean  # Remove object files
make fclean # Remove object files and the executable
make re     # Full rebuild
```

### Execution

```bash
./minishell
```

You will be presented with an interactive prompt. Type commands as you would in Bash:

```
minishell$ echo hello world
hello world
minishell$ ls -la | grep src
minishell$ cat << EOF
> line one
> line two
> EOF
minishell$ export FOO=bar
minishell$ echo $FOO
bar
minishell$ exit
```

## Architecture

```
src/
├── core/       Main loop, initialization, error handling
├── lexer/      Tokenization of input into words, pipes, redirections
├── parser/     Construction of a pipeline AST from tokens
├── expand/     Variable expansion ($VAR, $?)
├── exec/       Fork, pipe, execve, and PATH resolution
├── heredoc/    Here-document collection and signal-safe I/O
├── builtin/    Built-in command implementations
├── env/        Environment variable storage and access
├── signal/     Signal handlers for interactive, heredoc, and child modes
└── mem/        Arena-based memory allocator
```

## Resources
- [The Architecture of Open Source Applications](https://aosabook.org/en/v1/bash.html)
- [The Open Group Base Specifications - Shell Command Language](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html)
- [Bash Reference Manual](https://www.gnu.org/software/bash/manual/bash.html)
- [GNU Readline Library](https://tiswww.case.edu/php/chet/readline/rltop.html)
- [Writing Your Own Shell (Stephen Brennan)](https://brennan.io/2015/01/16/write-a-shell-in-c/)
- `man 2 fork`, `man 2 execve`, `man 2 pipe`, `man 2 dup2`, `man 2 waitpid`, `man 3 readline`

### AI Usage

AI (Claude) was used as a development aid throughout this project for the following purposes:

- **Code generation**: Writing initial implementations of core modules based on detailed specifications
- **Debugging**: Diagnosing signal handling issues (particularly heredoc + SIGINT interactions) and memory management problems
- **Documentation**: Generating internal documentation for complex subsystems (e.g., heredoc signal refactoring)
- **Code review**: Identifying edge cases in built-in commands and redirect handling

All AI-generated code was reviewed, tested, and adapted to meet the project requirements and 42's Norm coding standard.
