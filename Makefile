CC=gcc
ICDIRS=-Iinclude
OPT=-O1
CFLAGS=-Wall -Wextra -g $(INCDIRS) $(OPT)

CFILES=src/shell.c src/main.c
TESTFILES=tests/test.c
BINARY=bin/shell
TESTEXEC=tests/test

all: $(BINARY)

$(BINARY):
	$(CC) $(CFLAGS) $(CFILES) -o $@

run:
	$(BINARY)

$(TESTEXEC):
	$(CC) $(CFLAGS) src/shell.c $(TESTFILES) -o $@

test:
	$(TESTEXEC)

clean:
	rm -f $(BINARY)
	rm -rf $(TESTEXEC)
