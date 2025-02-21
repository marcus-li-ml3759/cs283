#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}



@test "External command: uname returns valid output" {
	run ./dsh <<EOF
uname
EOF
	[ "$status" -eq 0 ]
	[ -n "$output" ]
}

@test "Which command: 'which which' returns /usr/bin/which" {
	run ./dsh << EOF
which which
EOF
	result=$(echo "$output" | sed '/^dsh2>/d' | sed '/^$/d' | head -n 1)
	[[ "$result" == "/usr/bin/which"* ]]
}

@test "Echo with quoted spaces preserves internal spacing" {
    run ./dsh <<EOF
echo "  hello    world  "
EOF
    result=$(echo "$output" | grep -v "^dsh2>" | grep -v "^$")
    # Check that one of the lines exactly matches the expected string.
    echo "$result" | grep -Fxq "  hello    world  "
    [ "$?" -eq 0 ]
}

@test "check echo command" {
	run ./dsh <<EOF
echo Hello, World!
EOF
	[ "$status" -eq 0 ]
	[[ "${output}" == *"Hello, World!"* ]]
}

@test "check exit command" {
	run ./dsh <<EOF
exit
EOF
   [ "$status" -eq 0 ]
}



@test "Check cd changes directory" {
	run ./dsh <<EOF
cd /
pwd
EOF

	[ "$status" -eq 0 ]
	[[ "${output}" == *"/"* ]]
}

@test "Check cd with no argument" {
	run ./dsh <<EOF
cd
EOF

	[ "$status" -eq 0 ]
}



@test "Check multiple commands execution" {
	run ./dsh << EOF
echo First Command
ls
EOF

	[ "$status" -eq 0 ]
	[[ "${output}" == *"First Command"* ]]
}

@test "Empty input should not break shell" {
	run ./dsh <<EOF

EOF

	[ "$status" -eq 0 ]
}

@test "Testing echo command with extra leading/trailing whitespace" {
	run ./dsh <<EOF
	echo hello
EOF
	[ "$status" -eq 0 ]
	result=$(echo "$output" | sed '/^dsh2>/d' | sed '/^$/d' | head -n 1)
	[ "$result" = "hello" ]
}

@test "ls -l returns detailed listing" {
	run ./dsh <<EOF
ls -l
EOF
	[ "$status" -eq 0 ]
	echo "$output" | grep -q "total"
	[ "$?" -eq 0 ]
}

@test "Testing built-in cd with non-existent directory" {
   run ./dsh <<EOF
cd non_existent_dir
EOF
	[ "$status" -eq 0 ]
	echo "$output" | grep -qi "No such file"
	[ "$?" -eq 0 ]
}
