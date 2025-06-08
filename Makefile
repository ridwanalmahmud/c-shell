CC=gcc
ICDIRS=-I
OPT=-O1
CFLAGS=-Wall -Wextra -g $(INCDIRS) $(OPT)

CFILES=src/shell.c src/main.c
BINARY=bin/shell

all: $(BINARY)

$(BINARY):
	$(CC) $(CFLAGS) $(CFILES) -o $@

run:
	$(BINARY)

clean:
	rm -f $(BINARY)
