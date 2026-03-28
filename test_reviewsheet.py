#!/usr/bin/env python3
"""
Minishell Review Sheet Test Runner
Based on: /home/stanizak/Downloads/minishell-reviews/reviewsheet.html

Usage:
  python3 test_reviewsheet.py [--minishell PATH] [--section SECTION] [--verbose]

Sections:
  mandatory  - Run all mandatory tests
  bonus      - Run all bonus tests
  all        - Run everything (default)
  <name>     - Run specific section (e.g., echo, pipes, redirection)
"""

import subprocess
import os
import sys
import tempfile
import argparse
import shutil
from dataclasses import dataclass, field
from typing import Optional

# ── Configuration ────────────────────────────────────────────────────────────

MINISHELL   = "/home/stanizak/minishells/claude/minishell"
BASH        = "/bin/bash"
BASH_TESTS  = "/home/stanizak/minishells/bash/tests"

# ── Helpers ──────────────────────────────────────────────────────────────────

@dataclass
class TestResult:
    name: str
    passed: bool
    bash_out: str = ""
    mini_out: str = ""
    bash_rc: int = 0
    mini_rc: int = 0
    note: str = ""


def strip_echoed_commands(output: str, commands: list) -> str:
    """
    claude/minishell echoes the first line of each command before its output
    when running non-interactively.  Strip those echo lines in order.
    Multi-line commands (heredoc) are echoed only by their first line.
    """
    lines = output.split("\n")
    result = []
    # For each command, what minishell echoes is the first line only
    first_lines = [cmd.split("\n")[0] for cmd in commands if cmd.strip()]
    cmd_idx = 0
    for line in lines:
        if cmd_idx < len(first_lines) and line == first_lines[cmd_idx]:
            cmd_idx += 1
        else:
            result.append(line)
    return "\n".join(result)


def run_shell(commands: list, shell: str, env: Optional[dict] = None,
              cwd: Optional[str] = None, timeout: int = 5) -> tuple:
    """
    Run a list of commands in shell, return (stdout, stderr, returncode).
    """
    input_str = "\n".join(commands) + "\n"
    run_env = os.environ.copy()
    if env:
        run_env.update(env)
    try:
        proc = subprocess.run(
            [shell],
            input=input_str,
            capture_output=True,
            text=True,
            timeout=timeout,
            env=run_env,
            cwd=cwd,
        )
        stdout = proc.stdout
        if shell == MINISHELL:
            stdout = strip_echoed_commands(stdout, commands)
        return stdout, proc.stderr, proc.returncode
    except subprocess.TimeoutExpired:
        return "[TIMEOUT]", "", -1


def compare(name: str, commands: list, check_rc: bool = False,
            env: Optional[dict] = None, cwd: Optional[str] = None,
            note: str = "") -> TestResult:
    """
    Run commands in bash and minishell; compare stdout (and optionally rc).
    """
    b_out, b_err, b_rc = run_shell(commands, BASH,      env=env, cwd=cwd)
    m_out, m_err, m_rc = run_shell(commands, MINISHELL, env=env, cwd=cwd)

    b_norm = b_out.strip()
    m_norm = m_out.strip()
    passed = (b_norm == m_norm)
    if check_rc:
        passed = passed and (b_rc == m_rc)

    return TestResult(name=name, passed=passed,
                      bash_out=b_norm, mini_out=m_norm,
                      bash_rc=b_rc, mini_rc=m_rc, note=note)


def compare_rc(name: str, commands: list, note: str = "") -> TestResult:
    """Compare only the exit code (useful when stdout varies by env)."""
    _, _, b_rc = run_shell(commands, BASH)
    _, _, m_rc = run_shell(commands, MINISHELL)
    passed = (b_rc == m_rc)
    return TestResult(name=name, passed=passed,
                      bash_rc=b_rc, mini_rc=m_rc, note=note)


def manual(name: str, description: str) -> TestResult:
    """Placeholder for tests that require manual / interactive verification."""
    return TestResult(name=name, passed=None, note=f"[MANUAL] {description}")


# ── Test Sections ─────────────────────────────────────────────────────────────

def test_simple_command() -> list:
    results = []
    results.append(compare("simple: /bin/ls (no crash)",
                            ["/bin/ls /tmp > /dev/null", "echo ok"]))
    results.append(compare("simple: /bin/echo hello",
                            ["/bin/echo hello"]))
    results.append(compare("simple: /usr/bin/wc -l /etc/hostname",
                            ["/usr/bin/wc -l /etc/hostname"]))
    results.append(compare("simple: empty command (just newline)",
                            [""]))
    results.append(compare("simple: spaces-only command",
                            ["     "]))
    results.append(compare_rc("simple: invalid command exit code",
                              ["nonexistentcommand_xyz"]))
    return results


def test_arguments() -> list:
    results = []
    results.append(compare("args: /bin/echo with multiple args",
                            ["/bin/echo foo bar baz"]))
    results.append(compare("args: /bin/ls -l /tmp > /dev/null; echo ok",
                            ["/bin/ls -l /tmp > /dev/null", "echo ok"]))
    results.append(compare("args: /usr/bin/printf with args",
                            ["/usr/bin/printf '%s %s\\n' hello world"]))
    results.append(compare("args: many arguments",
                            ["/bin/echo a b c d e f g h i j k l m n o p"]))
    return results


def test_echo() -> list:
    """
    bash/tests reference: builtins.tests (echo section), quote.tests
    """
    results = []
    results.append(compare("echo: no args",           ["echo"]))
    results.append(compare("echo: simple string",     ["echo hello"]))
    results.append(compare("echo: multiple args",     ["echo hello world"]))
    results.append(compare("echo: -n flag",           ["echo -n hello"]))
    results.append(compare("echo: -n with space",     ["echo -n 'hello world'"]))
    results.append(compare("echo: empty string",      ["echo ''"]))
    results.append(compare("echo: literal backslash-n in dquote", ['echo "line1\\nline2"']))
    results.append(compare("echo: literal backslash-t in dquote", ['echo "col1\\tcol2"']))
    results.append(compare("echo: -n multiple args",  ["echo -n a b c"]))
    return results


def test_exit() -> list:
    """
    bash/tests reference: (no dedicated file; exit is tested in many .sub files)
    """
    results = []
    # exit code is checked via process returncode
    for code in [0, 1, 2, 42, 127]:
        _, _, rc = run_shell([f"exit {code}"], MINISHELL)
        passed = (rc == code)
        results.append(TestResult(
            name=f"exit: code {code}", passed=passed,
            mini_rc=rc, bash_rc=code))
    results.append(compare_rc("exit: no args (should exit 0)",
                               ["exit"]))
    results.append(compare_rc("exit: after command",
                               ["echo hi", "exit 5"]))
    return results


def test_return_value() -> list:
    """
    bash/tests reference: invert.tests
    """
    results = []
    results.append(compare("$?: after success",
                            ["echo hello", "echo $?"]))
    results.append(compare_rc("$?: after failure",
                              ["/bin/ls /nonexistent_xyz", "echo $?"]))
    results.append(compare("$?: /bin/ls success",
                            ["/bin/ls /tmp > /dev/null", "echo $?"]))
    results.append(compare("$?: false command exit 1",
                            ["/bin/false", "echo $?"]))
    results.append(compare("$?: true command exit 0",
                            ["/bin/true", "echo $?"]))
    results.append(compare("$?: double $? (expr $? + $?)",
                            ["/bin/false", "echo $? $?"]))
    results.append(compare("$?: after invalid command",
                            ["nonexistent_cmd_xyz", "echo $?"]))
    return results


def test_signals() -> list:
    """
    Signals require interactive/manual testing.
    bash/tests reference: (no automatable equivalent)
    """
    return [
        manual("signals: ctrl-C on empty prompt -> new prompt",
               "Run minishell, press ctrl-C: expect new prompt, no execution"),
        manual("signals: ctrl-\\ on empty prompt -> no action",
               "Press ctrl-\\: nothing should happen"),
        manual("signals: ctrl-D on empty prompt -> quit",
               "Press ctrl-D: minishell should exit cleanly"),
        manual("signals: ctrl-C after typing -> clear buffer",
               "Type 'echo hi', press ctrl-C, then Enter: nothing executes"),
        manual("signals: ctrl-D mid-input -> no action",
               "Type partial input, press ctrl-D: nothing happens"),
        manual("signals: ctrl-C during 'cat' (blocking) -> interrupt",
               "Run 'cat' (no args), press ctrl-C: cat terminates, new prompt"),
        manual("signals: ctrl-\\ during 'cat' -> quit signal",
               "Run 'cat', press ctrl-\\: process quits"),
        manual("signals: ctrl-D during 'cat' -> EOF to process",
               "Run 'cat', press ctrl-D: cat gets EOF and exits"),
    ]


def test_double_quotes() -> list:
    """
    bash/tests reference: quote.tests, nquote.tests
    """
    results = []
    results.append(compare('dquote: simple',
                            ['echo "hello world"']))
    results.append(compare('dquote: preserve spaces',
                            ['echo "  leading and trailing  "']))
    results.append(compare('dquote: with pipe chars (literal)',
                            ['echo "cat lol.c | cat > lol.c"']))
    results.append(compare('dquote: with semicolon (literal)',
                            ['echo "a ; b"']))
    results.append(compare('dquote: empty',
                            ['echo ""']))
    results.append(compare('dquote: multiword arg',
                            ['echo "foo" "bar"']))
    results.append(compare('dquote: escape backslash',
                            ['echo "back\\\\slash"']))
    results.append(compare('dquote: $VAR expanded inside',
                            ['export TESTVAR=hello', 'echo "$TESTVAR"']))
    return results


def test_single_quotes() -> list:
    """
    bash/tests reference: quote.tests
    """
    results = []
    results.append(compare("squote: simple",
                            ["echo 'hello world'"]))
    results.append(compare("squote: empty",
                            ["echo ''"]))
    results.append(compare("squote: $USER not expanded",
                            ["echo '$USER'"]))
    results.append(compare("squote: pipe char literal",
                            ["echo 'cat | grep'"]))
    results.append(compare("squote: redirection literal",
                            ["echo 'a > b'"]))
    results.append(compare("squote: env var literal",
                            ["echo '$HOME $PATH'"]))
    results.append(compare("squote: whitespace preserved",
                            ["echo '   spaces   '"]))
    return results


def test_env() -> list:
    """
    bash/tests reference: varenv.tests
    """
    results = []
    # env should contain PATH and HOME
    m_out, _, _ = run_shell(["env"], MINISHELL)
    b_out, _, _ = run_shell(["env"], BASH)

    has_path = "PATH=" in m_out
    has_home = "HOME=" in m_out
    results.append(TestResult("env: output contains PATH",
                               passed=has_path, mini_out=m_out))
    results.append(TestResult("env: output contains HOME",
                               passed=has_home, mini_out=m_out))
    # env in subshell shouldn't have bash-only vars
    results.append(compare_rc("env: exits 0", ["env > /dev/null"]))
    return results


def test_export() -> list:
    """
    bash/tests reference: varenv.tests, varenv*.sub
    """
    results = []
    results.append(compare("export: new variable visible in env",
                            ["export MYVAR=hello42", "env | grep MYVAR"]))
    results.append(compare("export: overwrite existing",
                            ["export PATH=/bin", "echo $PATH"]))
    results.append(compare("export: empty value",
                            ["export EMPTYVAR=", "echo $EMPTYVAR"]))
    results.append(compare("export: used in child command",
                            ["export GREETING=hello", "/bin/echo $GREETING"]))
    results.append(compare("export: multiple vars",
                            ["export A=1 B=2", "echo $A $B"]))
    return results


def test_unset() -> list:
    """
    bash/tests reference: varenv.tests
    """
    results = []
    results.append(compare("unset: remove exported var",
                            ["export RMVAR=remove_me",
                             "unset RMVAR",
                             "echo $RMVAR"]))
    results.append(compare("unset: var gone from env",
                            ["export RMVAR2=x",
                             "unset RMVAR2",
                             "echo $RMVAR2"]))
    results.append(compare("unset: unset nonexistent (no error)",
                            ["unset NONEXISTENT_VARXYZ", "echo ok"]))
    return results


def test_cd() -> list:
    """
    bash/tests reference: builtins.tests (cd section)
    """
    results = []
    results.append(compare("cd: to /tmp then pwd",
                            ["cd /tmp", "pwd"]))
    results.append(compare("cd: to /",
                            ["cd /", "pwd"]))
    results.append(compare("cd: relative ..",
                            ["cd /tmp", "cd ..", "pwd"]))
    results.append(compare("cd: nonexistent dir (error)",
                            ["cd /nonexistent_xyz_dir", "echo $?"]))
    results.append(compare("cd: HOME (no arg)",
                            ["cd", "pwd"]))
    results.append(compare("cd: '.'",
                            ["cd /tmp", "cd .", "pwd"]))
    # complex relative path
    results.append(compare("cd: complex relative ../../..",
                            ["cd /usr/share/doc", "cd ../../..", "pwd"]))
    return results


def test_pwd() -> list:
    results = []
    results.append(compare("pwd: in /tmp",     ["cd /tmp", "pwd"]))
    results.append(compare("pwd: in /",        ["cd /",    "pwd"]))
    results.append(compare("pwd: in /usr/bin", ["cd /usr/bin", "pwd"]))
    results.append(compare_rc("pwd: exit 0",   ["pwd > /dev/null"]))
    return results


def test_relative_path() -> list:
    # Create a temp script to execute via relative path
    results = []
    with tempfile.TemporaryDirectory() as tmpdir:
        script = os.path.join(tmpdir, "hello.sh")
        with open(script, "w") as f:
            f.write("#!/bin/sh\necho hello_from_script\n")
        os.chmod(script, 0o755)

        results.append(compare("relpath: ./script",
                                [f"cd {tmpdir}", "./hello.sh"]))
        results.append(compare("relpath: ../dir/script",
                                [f"cd {tmpdir}",
                                 "mkdir -p subdir",
                                 "cd subdir",
                                 "../hello.sh"]))
    results.append(compare("relpath: /bin/ls via PATH",
                            ["ls /tmp > /dev/null", "echo $?"]))
    return results


def test_env_path() -> list:
    """
    bash/tests reference: varenv.tests (PATH section)
    """
    results = []
    results.append(compare("PATH: ls works with default PATH",
                            ["ls /tmp > /dev/null", "echo ok"]))
    results.append(compare_rc("PATH: unset PATH -> command not found",
                              ["unset PATH", "ls"]))
    results.append(compare("PATH: multi-dir PATH (left to right)",
                            ["export PATH=/bin:/usr/bin",
                             "echo $PATH"]))
    # After unsetting PATH, restore is implied (each test is independent)
    return results


def test_redirection() -> list:
    """
    bash/tests reference: redir.tests, heredoc.tests, herestr.tests,
                          appendop.tests
    """
    results = []
    with tempfile.TemporaryDirectory() as tmpdir:
        out  = f"{tmpdir}/out.txt"
        inp  = f"{tmpdir}/in.txt"
        app  = f"{tmpdir}/app.txt"

        results.append(compare("redir: > (write)",
                                [f"echo hello > {out}", f"cat {out}"]))
        results.append(compare("redir: >> (append)",
                                [f"echo line1 > {app}",
                                 f"echo line2 >> {app}",
                                 f"cat {app}"]))
        results.append(compare("redir: < (read from file)",
                                [f"echo 'input line' > {inp}",
                                 f"cat < {inp}"]))
        results.append(compare("redir: < and > combined",
                                [f"echo hello > {inp}",
                                 f"cat < {inp} > {out}",
                                 f"cat {out}"]))
        results.append(compare("redir: >> multiple appends",
                                [f"echo a > {app}",
                                 f"echo b >> {app}",
                                 f"echo c >> {app}",
                                 f"cat {app}"]))
        results.append(compare("redir: << heredoc",
                                ["cat << EOF\nhello\nworld\nEOF"]))
        results.append(compare("redir: << heredoc with var expansion",
                                ["export HI=world",
                                 "cat << EOF\nhello $HI\nEOF"]))
    return results


def test_pipes() -> list:
    """
    bash/tests reference: posixpipe.tests, redir.tests, many *.tests
    """
    results = []
    results.append(compare("pipe: simple echo | grep",
                            ['echo banana | grep an']))
    results.append(compare("pipe: three-way pipe",
                            ['/bin/echo hello | cat | /usr/bin/wc -c']))
    results.append(compare("pipe: failing command in pipe",
                            ["/bin/ls /nonexistent_xyz | cat", "echo $?"]))
    results.append(compare("pipe: cat | cat | ls /tmp > /dev/null",
                            ["echo ok | cat | cat"]))
    results.append(compare("pipe: pipe + redirection",
                            [f"echo hello | cat > /tmp/pipe_redir_test_{os.getpid()}.txt",
                             f"cat /tmp/pipe_redir_test_{os.getpid()}.txt",
                             f"rm /tmp/pipe_redir_test_{os.getpid()}.txt"]))
    results.append(compare("pipe: many pipes",
                            ['echo hello | cat | cat | cat | cat']))
    results.append(compare("pipe: exit status of last command",
                            ["/bin/false | /bin/true", "echo $?"]))
    return results


def test_history() -> list:
    """
    History navigation (up/down arrows) is interactive-only.
    bash/tests reference: history.tests, histexp.tests
    """
    return [
        manual("history: Up/Down arrow navigates history",
               "Run several commands, press Up to recall them"),
        manual("history: ctrl-C clears buffer, Enter runs nothing",
               "Type 'echo hi', ctrl-C, then Enter: nothing executes"),
        manual("history: bad command -> error, no crash",
               "Run 'dsbksdgbksdghsd': minishell prints error and continues"),
        manual("history: cat | cat | ls behaves normally",
               "Run 'cat | cat | ls': ls output appears"),
    ]


def test_env_variables() -> list:
    """
    bash/tests reference: varenv.tests, exp.tests, new-exp.tests
    """
    results = []
    results.append(compare("envvar: $USER expands",
                            ["echo $USER"]))
    results.append(compare("envvar: $HOME expands",
                            ["echo $HOME"]))
    results.append(compare("envvar: $PATH expands",
                            ["echo $PATH"]))
    results.append(compare("envvar: double-quoted $USER expands",
                            ['echo "$USER"']))
    results.append(compare("envvar: undefined var -> empty",
                            ["echo $UNDEFINED_VAR_XYZ"]))
    results.append(compare("envvar: $? is numeric",
                            ["/bin/true", "echo $?"]))
    results.append(compare("envvar: export then use",
                            ["export MYVAL=42", "echo $MYVAL"]))
    results.append(compare("envvar: var in middle of string",
                            ['export A=hello', 'echo "${A}world"']))
    return results


# ── Bonus Sections ────────────────────────────────────────────────────────────

def test_and_or() -> list:
    """
    bash/tests reference: (no dedicated file; tested implicitly in many .tests)
    """
    results = []
    results.append(compare("&&: both succeed",
                            ["/bin/true && echo yes"]))
    results.append(compare("&&: first fails -> second skipped",
                            ["/bin/false && echo yes", "echo $?"]))
    results.append(compare("||: first fails -> second runs",
                            ["/bin/false || echo fallback"]))
    results.append(compare("||: first succeeds -> second skipped",
                            ["/bin/true || echo fallback"]))
    results.append(compare("&&||: chained",
                            ["/bin/false || echo a && echo b"]))
    results.append(compare("(): subshell grouping",
                            ["(echo sub1; echo sub2)"]))
    results.append(compare("(): subshell does not affect parent",
                            ["(export SUBVAR=x)", "echo $SUBVAR"]))
    return results


def test_wildcard() -> list:
    """
    bash/tests reference: glob.tests, extglob.tests
    """
    results = []
    with tempfile.TemporaryDirectory() as tmpdir:
        for name in ["apple.txt", "banana.txt", "cherry.md"]:
            open(os.path.join(tmpdir, name), "w").close()

        results.append(compare("wildcard: * matches all",
                                [f"cd {tmpdir}", "ls *"],
                                cwd=tmpdir))
        results.append(compare("wildcard: *.txt matches txt only",
                                [f"cd {tmpdir}", "ls *.txt"],
                                cwd=tmpdir))
        results.append(compare("wildcard: *.md matches md only",
                                [f"cd {tmpdir}", "ls *.md"],
                                cwd=tmpdir))
        results.append(compare("wildcard: no match -> literal",
                                [f"cd {tmpdir}", "ls *.xyz || echo no_match"],
                                cwd=tmpdir))
    return results


def test_surprise() -> list:
    """
    Bonus: mixed single/double quote nesting.
    bash/tests reference: quote.tests, nquote.tests
    """
    results = []
    results.append(compare("surprise: echo \"'$USER'\" -> value",
                            ["echo \"'$USER'\""]))
    results.append(compare("surprise: echo '\"$USER\"' -> literal $USER",
                            ["echo '\"$USER\"'"]))
    results.append(compare("surprise: single inside double",
                            ['echo "it\'s a test"']))
    results.append(compare("surprise: $? inside double quote",
                            ['/bin/true', 'echo "exit: $?"']))
    return results


# ── bash/tests File Mapping ───────────────────────────────────────────────────

BASH_TESTS_MAPPING = {
    "echo": [
        "builtins.tests",       # echo, printf builtins
        "quote.tests",          # quote handling with echo
        "nquote.tests",         # null-quote edge cases
    ],
    "exit": [
        "builtins.tests",       # exit is tested in general builtins
        "errors.tests",         # exit codes on errors
    ],
    "return_value": [
        "invert.tests",         # ! and $? tests
        "set-e.tests",          # exit on error
        "errors.tests",
    ],
    "double_quotes": [
        "quote.tests",
        "nquote.tests",
        "nquote1.tests",
        "nquote2.tests",
        "iquote.tests",         # special quoting
    ],
    "single_quotes": [
        "quote.tests",
        "nquote.tests",
    ],
    "env_export_unset": [
        "varenv.tests",
        "varenv1.sub",
        "varenv2.sub",
        "varenv3.sub",
        "attr.tests",           # variable attributes (export)
    ],
    "cd_pwd": [
        "builtins.tests",       # cd, pwd, pushd/popd
        "dstack.tests",         # dirstack (pushd/popd)
    ],
    "redirection": [
        "redir.tests",
        "redir1.sub",
        "redir2.sub",
        "heredoc.tests",
        "heredoc1.sub",
        "heredoc2.sub",
        "herestr.tests",
        "appendop.tests",       # >> operator
    ],
    "pipes": [
        "posixpipe.tests",
        "comsub.tests",         # command substitution with pipes
        "lastpipe.tests",       # lastpipe behavior
    ],
    "env_variables": [
        "varenv.tests",
        "exp.tests",            # variable expansion
        "new-exp.tests",        # new expansion features
        "more-exp.tests",
        "posixexp.tests",
    ],
    "wildcards": [
        "glob.tests",
        "glob1.sub",
        "glob2.sub",
        "extglob.tests",
        "extglob1.sub",
    ],
    "and_or": [
        "cond.tests",           # conditional expressions
        "posix2.tests",         # POSIX && and ||
    ],
    "history": [
        "history.tests",
        "history1.sub",
        "histexp.tests",        # history expansion
    ],
}


def print_bash_tests_mapping():
    print("\n" + "=" * 60)
    print("bash/tests -> Review Sheet mapping")
    print("=" * 60)
    for section, files in BASH_TESTS_MAPPING.items():
        print(f"\n[{section}]")
        for f in files:
            path = os.path.join(BASH_TESTS, f)
            exists = "✓" if os.path.exists(path) else "✗ (not found)"
            print(f"  {exists}  {f}")


# ── Runner ────────────────────────────────────────────────────────────────────

SECTIONS = {
    "simple_command":  ("Simple Command & global variables", test_simple_command),
    "arguments":       ("Arguments",                         test_arguments),
    "echo":            ("echo",                              test_echo),
    "exit":            ("exit",                              test_exit),
    "return_value":    ("Return value of a process ($?)",    test_return_value),
    "signals":         ("Signals",                           test_signals),
    "double_quotes":   ("Double Quotes",                     test_double_quotes),
    "single_quotes":   ("Single Quotes",                     test_single_quotes),
    "env":             ("env",                               test_env),
    "export":          ("export",                            test_export),
    "unset":           ("unset",                             test_unset),
    "cd":              ("cd",                                test_cd),
    "pwd":             ("pwd",                               test_pwd),
    "relative_path":   ("Relative Path",                     test_relative_path),
    "env_path":        ("Environment path ($PATH)",          test_env_path),
    "redirection":     ("Redirection",                       test_redirection),
    "pipes":           ("Pipes",                             test_pipes),
    "history":         ("Go Crazy and history",              test_history),
    "env_variables":   ("Environment variables",             test_env_variables),
    # Bonus
    "and_or":          ("[BONUS] And, Or (&&, ||, ())",      test_and_or),
    "wildcard":        ("[BONUS] Wildcard (*)",               test_wildcard),
    "surprise":        ("[BONUS] Surprise! (mixed quotes)",  test_surprise),
}

MANDATORY_SECTIONS = [
    "simple_command", "arguments", "echo", "exit", "return_value",
    "signals", "double_quotes", "single_quotes", "env", "export",
    "unset", "cd", "pwd", "relative_path", "env_path",
    "redirection", "pipes", "history", "env_variables",
]

BONUS_SECTIONS = ["and_or", "wildcard", "surprise"]

GREEN  = "\033[92m"
RED    = "\033[91m"
YELLOW = "\033[93m"
CYAN   = "\033[96m"
RESET  = "\033[0m"
BOLD   = "\033[1m"


def print_result(r: TestResult, verbose: bool):
    if r.passed is None:
        print(f"  {YELLOW}[MANUAL]{RESET} {r.name}")
        if verbose:
            print(f"    {r.note}")
    elif r.passed:
        print(f"  {GREEN}[PASS]{RESET}   {r.name}")
    else:
        print(f"  {RED}[FAIL]{RESET}   {r.name}")
        if verbose or True:
            print(f"    bash:      {repr(r.bash_out[:120])}")
            print(f"    minishell: {repr(r.mini_out[:120])}")
            if r.bash_rc != r.mini_rc and r.bash_rc != 0:
                print(f"    rc bash={r.bash_rc}  mini={r.mini_rc}")


def run_sections(section_keys: list, verbose: bool = False) -> tuple:
    total = passed = failed = manual_count = 0
    for key in section_keys:
        label, fn = SECTIONS[key]
        print(f"\n{BOLD}{CYAN}── {label} ──{RESET}")
        results = fn()
        for r in results:
            print_result(r, verbose)
            if r.passed is None:
                manual_count += 1
            elif r.passed:
                passed += 1
                total += 1
            else:
                failed += 1
                total += 1
    return total, passed, failed, manual_count


def main():
    global MINISHELL
    parser = argparse.ArgumentParser(description="Minishell review sheet tester")
    parser.add_argument("--minishell", default=MINISHELL,
                        help="Path to minishell binary")
    parser.add_argument("--section", default="all",
                        help="Section to run: all, mandatory, bonus, or section name")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="Show extra detail on failures")
    parser.add_argument("--list-mapping", action="store_true",
                        help="Show bash/tests -> review-sheet mapping and exit")
    args = parser.parse_args()

    MINISHELL = args.minishell

    if not os.path.exists(MINISHELL):
        print(f"{RED}Error: minishell not found at {MINISHELL}{RESET}")
        sys.exit(1)

    if args.list_mapping:
        print_bash_tests_mapping()
        return

    if args.section == "all":
        keys = MANDATORY_SECTIONS + BONUS_SECTIONS
    elif args.section == "mandatory":
        keys = MANDATORY_SECTIONS
    elif args.section == "bonus":
        keys = BONUS_SECTIONS
    elif args.section in SECTIONS:
        keys = [args.section]
    else:
        print(f"Unknown section '{args.section}'. Available:")
        for k in SECTIONS:
            print(f"  {k}")
        sys.exit(1)

    print(f"{BOLD}Minishell Review Sheet Tester{RESET}")
    print(f"Binary: {MINISHELL}")
    print(f"Section: {args.section}")

    total, passed, failed, manual_count = run_sections(keys, args.verbose)

    print(f"\n{BOLD}{'=' * 50}{RESET}")
    print(f"{BOLD}Results:{RESET}")
    print(f"  {GREEN}Passed:  {passed}/{total}{RESET}")
    print(f"  {RED}Failed:  {failed}/{total}{RESET}")
    print(f"  {YELLOW}Manual:  {manual_count} (require interactive verification){RESET}")
    if total > 0:
        pct = passed * 100 // total
        bar = "#" * (pct // 5) + "." * (20 - pct // 5)
        print(f"  Score:   [{bar}] {pct}%")

    print(f"\nRun with --list-mapping to see bash/tests file associations.")


if __name__ == "__main__":
    main()
