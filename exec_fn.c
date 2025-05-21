#include "exec.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool
do_nf (Exec *exec)
{
  unsigned int pos = exec_pop_ret_stack (exec);
  if (*exec->error)
    {
      /* we are probably exiting from the main function */
      memset (exec->error, 0, sizeof (exec->error));
      return false;
    }

  exec->lexer->pos = pos;

  return true;
}

bool
do_fn (Exec *exec)
{
  Object *fn = malloc (sizeof (Object));
  Token name, token;
  fn->type = OBJ_FUNCTION;

  name = lexer_next (exec->lexer);
  DO_TEST_TOKEN (name, TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (lexer_next (exec->lexer), TOKEN_SEMICOLON);

  fn->name = name.as.str;
  fn->as.function.name = name.as.str;
  fn->as.function.pos = exec->lexer->pos + 1;

  /* append the function to the function list */
  exec->vars_capacity += sizeof (Object *);
  exec->vars = realloc (exec->vars, exec->vars_capacity);
  exec->vars[exec->vars_len] = fn;
  exec->vars_len++;

  /* skip to TOKEN_NF */
  do
    {
      token = lexer_next (exec->lexer);
    }
  while (token.type != TOKEN_NF);
  DO_TEST_TOKEN (lexer_next (exec->lexer), TOKEN_SEMICOLON);

  arena_append (&exec->objects_arena, fn);

  return true;
}

bool
do_call (Exec *exec)
{
  Token name;

  name = lexer_next (exec->lexer);
  DO_TEST_TOKEN (name, TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (lexer_next (exec->lexer), TOKEN_SEMICOLON);

  if (exec_fcall (exec, name.as.str) == FCALL_FAIL)
    return false;
  return true;
}
