#!/bin/sh
set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/minishell"
BASH_BIN="${BASH_BIN:-bash}"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
pass=0
fail=0

if [ ! -x "$BIN" ]; then
	echo "minishell binary not found: $BIN" >&2
	exit 1
fi

run_ms() {
	input="$1"
	out="$2"
	set +e
	printf "%b" "$input" | "$BIN" >"$out" 2>&1
	status=$?
	set -e
	echo "$status"
}

run_bash() {
	input="$1"
	out="$2"
	set +e
	printf "%b" "$input" | "$BASH_BIN" --noprofile --norc >"$out" 2>&1
	status=$?
	set -e
	echo "$status"
}

mark_pass() {
	name="$1"
	printf '[OK] %s\n' "$name"
	pass=$((pass + 1))
}

mark_fail() {
	name="$1"
	printf '[NG] %s\n' "$name"
	fail=$((fail + 1))
}

show_output() {
	out="$1"
	printf '  output:\n'
	sed 's/^/    /' "$out"
}

assert_exact() {
	name="$1"
	input="$2"
	expected="$3"
	expected_status="$4"
	out="$TMP/out.txt"
	status="$(run_ms "$input" "$out")"
	actual="$(cat "$out")"
	if [ "$status" = "$expected_status" ] && [ "$actual" = "$expected" ]; then
		mark_pass "$name"
	else
		mark_fail "$name"
		printf '  expected status: %s\n' "$expected_status"
		printf '  actual status  : %s\n' "$status"
		printf '  expected output:\n'
		printf '    %s\n' "$expected"
		show_output "$out"
	fi
}

assert_contains() {
	name="$1"
	input="$2"
	expected_status="$3"
	shift 3
	out="$TMP/out.txt"
	status="$(run_ms "$input" "$out")"
	ok=1
	if [ "$status" != "$expected_status" ]; then
		ok=0
		printf '  expected status: %s\n' "$expected_status"
		printf '  actual status  : %s\n' "$status"
	fi
	for pat in "$@"; do
		if ! grep -F -- "$pat" "$out" >/dev/null 2>&1; then
			ok=0
			printf '  missing pattern: %s\n' "$pat"
		fi
	done
	if [ "$ok" -eq 1 ]; then
		mark_pass "$name"
	else
		mark_fail "$name"
		show_output "$out"
	fi
}

assert_not_contains() {
	name="$1"
	input="$2"
	unexpected="$3"
	out="$TMP/out.txt"
	status="$(run_ms "$input" "$out")"
	if grep -F -- "$unexpected" "$out" >/dev/null 2>&1; then
		mark_fail "$name"
		printf '  unexpected pattern found: %s\n' "$unexpected"
		printf '  status: %s\n' "$status"
		show_output "$out"
	else
		mark_pass "$name"
	fi
}

assert_status_matches_bash() {
	name="$1"
	input="$2"
	ms_out="$TMP/ms.txt"
	bash_out="$TMP/bash.txt"
	ms_status="$(run_ms "$input" "$ms_out")"
	bash_status="$(run_bash "$input" "$bash_out")"
	if [ "$ms_status" = "$bash_status" ]; then
		mark_pass "$name"
	else
		mark_fail "$name"
		printf '  minishell status: %s\n' "$ms_status"
		printf '  bash status     : %s\n' "$bash_status"
		printf '  minishell output:\n'
		sed 's/^/    /' "$ms_out"
		printf '  bash output:\n'
		sed 's/^/    /' "$bash_out"
	fi
}

# Refs: bash/tests/{redir.tests,dollar.right,quote.tests}
assert_exact "echo_basic" "echo hello\nexit\n" "hello" "0"
assert_exact "pipeline_basic" "echo abc | cat\nexit\n" "abc" "0"
assert_exact "env_expand" "export FOO=bar\necho \$FOO\nexit\n" "bar" "0"
assert_exact "status_expand" "nosuchcmd\necho \$?\nexit\n" "minishell: nosuchcmd: command not found
127" "0"
assert_exact "redirect_in_out" "echo wow > $TMP/in\ncat < $TMP/in\nexit\n" "wow" "0"
assert_exact "redirect_append" "echo a > $TMP/f2\necho b >> $TMP/f2\ncat $TMP/f2\nexit\n" "a
b" "0"

# Ref: bash/tests/heredoc.tests (quoted vs unquoted delimiter)
assert_exact "heredoc_unquoted_expands" "export HD=ok\ncat << EOF\n\$HD\nEOF\nexit\n" "ok" "0"
assert_exact "heredoc_quoted_no_expand" "export HD=ok\ncat << 'EOF'\n\$HD\nEOF\nexit\n" "\$HD" "0"

# Ref: bash/tests/errors.tests / parser.tests
assert_contains "syntax_leading_pipe" "| cat\n" "2" "syntax error near unexpected token \`|'"
assert_contains "syntax_trailing_pipe" "echo hi |\n" "2" "syntax error near unexpected token \`newline'"
assert_contains "syntax_double_pipe" "echo a || cat\n" "2" "syntax error near unexpected token \`|'"
assert_contains "syntax_missing_redirect_target" "echo hi >\n" "2" "syntax error near unexpected token \`newline'"
assert_contains "syntax_redirect_after_redirect" "echo hi > > file\n" "2" "syntax error near unexpected token \`>'"
assert_contains "syntax_unclosed_single_quote" "echo 'abc\n" "2" "unexpected EOF while looking for matching" "syntax error: unexpected end of file"
assert_contains "syntax_unclosed_double_quote" "echo \"abc\n" "2" "unexpected EOF while looking for matching" "syntax error: unexpected end of file"

# Ref: bash/tests/errors.tests (command execution failures)
assert_contains "error_command_not_found" "nosuchcmd\n" "127" "command not found"
assert_contains "error_path_not_found" "/no/such/path\n" "127" "No such file or directory"
mkdir -p "$TMP/noexec_dir"
printf '#!/bin/sh\necho hi\n' >"$TMP/noexec.sh"
chmod 0644 "$TMP/noexec.sh"
assert_contains "error_permission_denied" "$TMP/noexec.sh\n" "126" "Permission denied"

# Ref: bash/tests/errors.tests (exit builtin behavior)
assert_contains "exit_too_many_arguments" "exit 1 2\necho still-running\nexit\n" "0" "too many arguments" "still-running"
assert_contains "exit_numeric_required" "exit abc\n" "2" "numeric argument required"
assert_not_contains "exit_numeric_required_stops_shell" "exit abc\necho should-not-print\n" "should-not-print"

# Empty line should not become syntax error, and previous status remains available.
assert_contains "empty_line_preserves_status" "nosuchcmd\n\necho \$?\nexit\n" "0" "127"

# Ref: bash/tests/misc/sigint-*.sh (foreground signal status)
assert_status_matches_bash "signal_sigint_status" "sh -c 'kill -INT \$\$'\n"
assert_status_matches_bash "signal_sigquit_status" "sh -c 'kill -QUIT \$\$'\n"
assert_contains "signal_sigquit_message" "sh -c 'kill -QUIT \$\$'\n" "131" "Quit: 3"
assert_contains "signal_sigint_updates_dollar_q" "sh -c 'kill -INT \$\$'\necho \$?\nexit\n" "0" "130"

printf 'passed=%d failed=%d\n' "$pass" "$fail"
if [ "$fail" -ne 0 ]; then
	exit 1
fi
