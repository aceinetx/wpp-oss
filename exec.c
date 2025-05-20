#include "exec.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_TOKEN(v, t)                                                      \
  if (v.type != t)                                                            \
    {                                                                         \
      sprintf (exec->error, "%d: syntax error", v.line);                      \
      return false;                                                           \
    }

static bool
do_fn (Exec *exec)
{
  Object fn;
  Token name, token;
  fn.type = OBJ_FUNCTION;

  name = lexer_next (exec->lexer);
  TEST_TOKEN (name, TOKEN_IDENTIFIER);

  fn.as.function.name = name.as.str;
  fn.as.function.pos = exec->lexer->pos + 1;

  /* append the function to the function list */
  exec->vars_capacity += sizeof (Object);
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

  return true;
}

static bool
do_println (Exec *exec)
{
  Token string;

  string = lexer_next (exec->lexer);
  TEST_TOKEN (string, TOKEN_STRING);

  printf ("%s\n", string.as.str);

  return true;
}

static bool
do_call (Exec *exec)
{
  Token name;

  name = lexer_next (exec->lexer);
  TEST_TOKEN (name, TOKEN_IDENTIFIER);

  if (exec_fcall (exec, name.as.str) == FCALL_FAIL)
    return false;
  return true;
}
#undef TEST_TOKEN

Exec *
exec_new (Lexer *lexer)
{
  Exec *exec = malloc (sizeof (Exec));
  memset (exec, 0, sizeof (Exec));

  exec->lexer = lexer;
  exec->strings_arena = arena_new ();
  return exec;
}

void
exec_free (Exec *exec)
{
  if (exec->vars)
    {
      free (exec->vars);
    }

  arena_free (&exec->strings_arena);
  free (exec);
}

int
exec_fcall (Exec *exec, const char *name)
{
  unsigned int i;
  for (i = 0; i < exec->vars_len; i++)
    {
      Object *obj = &exec->vars[i];
      if (obj->type == OBJ_FUNCTION)
        {
          if (strcmp (obj->as.function.name, name) == 0)
            {
              exec->lexer->pos = obj->as.function.pos;
              return FCALL_SUCCESS;
            }
        }
    }
  return FCALL_FAIL;
}

void
exec_run (Exec *exec)
{
  Token token;
  bool main_iteration = false;
  if (exec->lexer->pos > 0)
    main_iteration = true;

  token = lexer_next (exec->lexer);
  while (token.type != TOKEN_END)
    {
      if (token.type == TOKEN_FN)
        {
          if (!do_fn (exec))
            return;
        }
      else if (token.type == TOKEN_PRINTLN)
        {
          if (!do_println (exec))
            return;
        }
      else if (token.type == TOKEN_CALL)
        {
          if (!do_call (exec))
            return;
        }
      token = lexer_next (exec->lexer);
    }

  if (!main_iteration)
    {
      if (exec_fcall (exec, "main") == FCALL_FAIL)
        {
          sprintf (exec->error, "exec_fcall failed: no main function");
          return;
        }

      exec_run (exec);
    }
}
