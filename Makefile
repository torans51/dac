compile:
	cc -Wall -Wextra -std=c99 dac_test.c -o dac_test

test: compile
	echo "Running test ..." && ./dac_test
