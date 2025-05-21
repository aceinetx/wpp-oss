#include "exec.h"
#include "object.h"
#include "wpp.h"
#include <stdio.h>

bool
do_scanln (Exec *exec)
{
  Token dest;
  Object src;

  dest = lexer_next (exec->lexer);
  DO_TEST_TOKEN (dest, TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (lexer_next (exec->lexer), TOKEN_SEMICOLON);

  {
    char *buffer = arena_alloc (&exec->strings_arena, 256);
    fgets (buffer, 255, stdin);

    buffer[strcspn (buffer, "\n")] = 0;

    src.type = OBJ_STRING;
    src.as.string = buffer;
  }

  exec_assign (exec, dest.as.str, src);

  return true;
}
