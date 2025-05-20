.PHONY: all

CC = cc
CFLAGS = -std=c90 -Wall -Wextra -Wpedantic -Werror -O0 -ggdb

all: wpp
clean:
	rm -rf wpp *.o

# executables
wpp: wpp.o lexer.o arena.o exec.o object.o exec_print.o
	$(CC) -o $@ $^

# objects
wpp.o: wpp.c lexer.h exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

lexer.o: lexer.c lexer.h wpp.h
	$(CC) -c -o $@ $(CFLAGS) $<

arena.o: arena.c arena.h
	$(CC) -c -o $@ $(CFLAGS) $<

exec.o: exec.c exec.h wpp.h
	$(CC) -c -o $@ $(CFLAGS) $<

exec_print.o: exec_print.c exec.h wpp.h
	$(CC) -c -o $@ $(CFLAGS) $<

object.o: object.c object.h
	$(CC) -c -o $@ $(CFLAGS) $<

# headers
lexer.h: arena.h
	@touch $@

exec.h: lexer.h object.h
	@touch $@
