#include "exec.h"
#include "wpp.h"
#include <stdio.h>

bool
do_cp (Exec *exec)
{
  Token dest, src;
  Object *osrc;

  dest = lexer_next (exec->lexer);
  src = lexer_next (exec->lexer);
  DO_TEST_TOKEN (dest, TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (src, TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (lexer_next (exec->lexer), TOKEN_SEMICOLON);

  osrc = exec_getvar (exec, src.as.str);
  if (!osrc)
    {
      printf ("wpp: unknown var %s\n", src.as.str);
      return false;
    }

  exec_assign (exec, dest.as.str, *osrc);

  return true;
}
