.PHONY: all

CC = cc
CFLAGS = -std=c90 -Wall -Wextra -Wpedantic -Werror -Oz -ggdb
LDFLAGS =

all: wpp
clean:
	rm -rf wpp *.o

# executables
wpp: wpp.o lexer.o arena.o exec.o object.o exec_print.o exec_fn.o exec_var.o exec_copy.o exec_scanln.o exec_ret_stack.o exec_obj.o
	$(CC) $(LDFLAGS) -o $@ $^

# objects
wpp.o: wpp.c lexer.h exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

lexer.o: lexer.c lexer.h wpp.h
	$(CC) -c -o $@ $(CFLAGS) $<

arena.o: arena.c arena.h
	$(CC) -c -o $@ $(CFLAGS) $<

exec.o: exec.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

exec_print.o: exec_print.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

exec_fn.o: exec_fn.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

exec_var.o: exec_var.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

exec_copy.o: exec_copy.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

exec_scanln.o: exec_scanln.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

exec_ret_stack.o: exec_ret_stack.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

exec_obj.o: exec_obj.c exec.h
	$(CC) -c -o $@ $(CFLAGS) $<

object.o: object.c object.h
	$(CC) -c -o $@ $(CFLAGS) $<

# headers
lexer.h: arena.h
	@touch $@

exec.h: lexer.h object.h wpp.h
	@touch $@
