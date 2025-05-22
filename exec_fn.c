#include "exec.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool
wpp_do_nf (wppExec *exec)
{
  unsigned int pos = wpp_exec_pop_ret_stack (exec);
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
wpp_do_fn (wppExec *exec)
{
  wppObject *fn = malloc (sizeof (wppObject));
  wppToken name, token;
  fn->type = WPP_OBJ_FUNCTION;

  name = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (name, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  fn->name = name.as.str;
  fn->as.function.pos = exec->lexer->pos + 1;

  /* append the function to the function list */
  exec->vars_capacity += sizeof (wppObject *);
  exec->vars = realloc (exec->vars, exec->vars_capacity);
  exec->vars[exec->vars_len] = fn;
  exec->vars_len++;

  /* skip to WPP_TOKEN_NF */
  do
    {
      token = wpp_lexer_next (exec->lexer);
    }
  while (token.type != WPP_TOKEN_NF);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  wpp_arena_append (&exec->objects_arena, fn);

  return true;
}

bool
wpp_do_call (wppExec *exec)
{
  wppToken name;

  name = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (name, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  if (wpp_exec_fcall (exec, name.as.str) == FCALL_FAIL)
    return false;
  return true;
}
