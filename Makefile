CC     := gcc
CFLAGS := -std=c89 -Wall -Iinclude


.PHONY: all


all: tests/tokenise.exe tests/token_as_float.exe


tests/tokenise.exe: src/parse/tokens.c tests/tokenise.c
	$(CC) $(CFLAGS) $^ -o $@

tests/token_as_float.exe: src/parse/tokens.c src/parse/token_value.c tests/token_as_float.c
	$(CC) $(CFLAGS) $^ -o $@
