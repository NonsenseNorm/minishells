#!/bin/sh
    MINISHELL=./minishell
    PASS=0
    FAIL=0

    run_test() {
        local desc="$1"
        local input="$2"
        local setup="$3"
        local check="$4"

        # setup
        eval "$setup" 2>/dev/null

        # bash
        bash_out=$(echo "$input" | bash 2>&1)
        bash_exit=$?
        eval "$setup" 2>/dev/null
        bash_file=$(eval "$check" 2>/dev/null)

        # minishell
        eval "$setup" 2>/dev/null
        mini_out=$(echo "$input" | $MINISHELL 2>&1)
        mini_exit=$?
        mini_file=$(eval "$check" 2>/dev/null)

        if [ "$bash_out" = "$mini_out" ] && [ "$bash_exit" = "$mini_exit" ] && [ "$bash_file" = "$mini_file" ]; then
            echo "PASS: $desc"
            PASS=$((PASS + 1))
        else
            echo "FAIL: $desc"
            echo "  [bash]  exit=$bash_exit stdout='$bash_out' file='$bash_file'"
            echo "  [mini]  exit=$mini_exit stdout='$mini_out' file='$mini_file'"
            FAIL=$((FAIL + 1))
        fi

        rm -f /tmp/test_rdwr.txt
    }

    # --- テストケース ---

    # 1. 存在するファイルを <> で開き cat で読む
    run_test "<> で既存ファイルを stdin から読む" \
        'cat <> /tmp/test_rdwr.txt' \
        'echo "hello" > /tmp/test_rdwr.txt' \
        'echo ""'   # stdout の比較のみ（bash は "hello" を出力するはず）

    # 2. 存在しないファイルに <> → ファイルが作成されるか
    run_test "<> で存在しないファイルを指定したとき作成されるか" \
        'cat <> /tmp/test_rdwr.txt' \
        'rm -f /tmp/test_rdwr.txt' \
        'test -f /tmp/test_rdwr.txt && echo "created" || echo "not created"'

    # 3. <> でファイルに書き込み (fd 0 経由で echo は書かないので、true で副作用だけ見る)
    run_test "<> でファイルを開いて終了ステータスが 0 か" \
        'true <> /tmp/test_rdwr.txt' \
        'echo "" > /tmp/test_rdwr.txt' \
        'echo ""'

    # 4. 存在しないファイルへの <> でエラーにならないか（bash はエラーなし・ファイル作成）
    run_test "<> 新規ファイル作成後のファイル存在確認" \
        'true <> /tmp/test_rdwr.txt' \
        'rm -f /tmp/test_rdwr.txt' \
        'test -f /tmp/test_rdwr.txt && echo "created" || echo "not created"'

    # 5. $? が正しいか
    run_test "<> 成功時の終了ステータス \$?" \
        'true <> /tmp/test_rdwr.txt; echo $?' \
        'echo "" > /tmp/test_rdwr.txt' \
        'echo ""'

    echo ""
    echo "Result: PASS=$PASS FAIL=$FAIL"
