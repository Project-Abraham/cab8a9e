CC     := gcc
CFLAGS := -std=c89 -Wall -Iinclude


tests/tokenise.exe: src/parse/tokens.c tests/tokenise.c
	$(CC) $(CFLAGS) $^ -o $@
