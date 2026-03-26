*This project has been created as part of the 42 curriculum by ayusa, fnakamur*

# Description

A minimal Unix shell implementation in C that replicates core bash behavior.

The shell reads user input interactively via `readline`, parses it into an Abstract Syntax Tree (AST), and executes commands with proper handling of pipes, redirections, environment variables, and signals. The pipeline is composed of four stages: **Lexer → Parser → Expander → Executor**.

**Supported features:**
- Pipes (`|`) and pipelines
- Input/output redirections (`<`, `>`, `>>`, `<>`)
- Heredoc (`<<`) with variable expansion (unless the delimiter is quoted)
- Logical operators (`&&`, `||`) with correct precedence
- Subshells via parentheses (`(...)`)
- Variable expansion (`$VAR`, `$?`)
- Wildcard expansion (`*`) in the current directory
- Single quotes (prevent all expansion) and double quotes (allow `$` expansion)
- Command history (arrow keys)
- Signal handling: `Ctrl+C` (SIGINT), `Ctrl+D` (EOF), `Ctrl+\` (SIGQUIT)

**Built-in commands:**
| Command | Description |
|---------|-------------|
| `echo [-n]` | Print arguments to stdout |
| `cd [dir]` | Change working directory |
| `pwd` | Print current working directory |
| `export [name[=value]]` | Set or display exported environment variables |
| `unset [name]` | Remove environment variables |
| `env` | Display all environment variables |
| `exit [n]` | Exit the shell with status `n` |

# Instructions

**Requirements**

- GCC or Clang
- GNU Readline library (`libreadline-dev` on Debian/Ubuntu)
- `make`

**Compilation**
```
make
```

**Execution**
```
./minishell
```

The shell launches with the prompt `minishell$`.

**Usage examples**
```sh
minishell$ echo "Hello, world!"
minishell$ ls -la | grep ".c" | wc -l
minishell$ cat < infile.txt > outfile.txt
minishell$ mkdir /tmp/testdir && echo "created"
minishell$ cat nonexistent.txt || echo "file not found"
minishell$ false || true && echo "success"
minishell$ export FOO=bar && echo $FOO
minishell$ (echo hello; echo world) | cat
minishell$ cat << EOF
> line1
> EOF
minishell$ echo $?
```

**Cleanup**
```
make fclean
```

# Resources
- [低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E7%94%9F%E6%88%90%E8%A6%8F%E5%89%87%E3%81%AB%E3%82%88%E3%82%8B%E6%96%87%E6%B3%95%E3%81%AE%E5%AE%9A%E7%BE%A9)

- [minishellで学んだこと](https://zenn.dev/khanadat/articles/79b570722a65f4)

- [JUNのブログ](https://jun-networks.hatenablog.com/entry/2021/07/02/034002)

## AI Usage

In this project, AI (Claude Code, Gemini) was used for the following tasks:
- Generating diverse test cases and edge-case prompts to verify that each shell feature (redirections, heredoc, wildcard, subshell) behaved correctly
- Providing sample usage of permitted library functions (e.g., `readline`, `tcgetattr`, `waitpid`) to clarify their interfaces before writing the actual implementation
- Norminette compliance review: checking that function lengths, variable counts, and formatting rules were respected during refactoring
- Receiving a wide variety of shell prompts to understand the features required by the assignment
- Obtaining simple sample code for permitted functions and running it hands-on to build understanding
