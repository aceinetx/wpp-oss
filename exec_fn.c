#include "exec.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>

bool
do_fn (Exec *exec)
{
  Object *fn = malloc (sizeof (Object));
  Token name, token;
  fn->type = OBJ_FUNCTION;

  name = lexer_next (exec->lexer);
  DO_TEST_TOKEN (name, TOKEN_IDENTIFIER);

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
  lexer_next (exec->lexer);

  arena_append (&exec->objects_arena, fn);

  return true;
}

bool
do_call (Exec *exec)
{
  Token name;

  name = lexer_next (exec->lexer);
  DO_TEST_TOKEN (name, TOKEN_IDENTIFIER);

  if (exec_fcall (exec, name.as.str) == FCALL_FAIL)
    return false;
  return true;
}
