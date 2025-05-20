#include "exec.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool do_print (Exec *exec);
bool do_println (Exec *exec);
bool do_fn (Exec *exec);
bool do_call (Exec *exec);

Exec *
exec_new (Lexer *lexer)
{
  Exec *exec = malloc (sizeof (Exec));
  memset (exec, 0, sizeof (Exec));

  exec->lexer = lexer;
  exec->objects_arena = arena_new ();
  return exec;
}

void
exec_free (Exec *exec)
{
  if (exec->vars)
    {
      free (exec->vars);
    }

  arena_free (&exec->objects_arena);
  free (exec);
}

int
exec_fcall (Exec *exec, const char *name)
{
  unsigned int i;
  for (i = 0; i < exec->vars_len; i++)
    {
      Object *obj = exec->vars[i];
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

Object *
exec_getvar (Exec *exec, const char *name)
{
  unsigned int i;
  for (i = 0; i < exec->vars_len; i++)
    {
      Object *obj = exec->vars[i];
      if (strcmp (obj->name, name) == 0)
        return obj;
    }
  return NULL;
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
#define DO_TOKEN(t, f)                                                        \
  if (token.type == t)                                                        \
    if (!f (exec))                                                            \
      return;

      DO_TOKEN (TOKEN_FN, do_fn);
      DO_TOKEN (TOKEN_PRINTLN, do_println);
      DO_TOKEN (TOKEN_PRINT, do_print);
      DO_TOKEN (TOKEN_CALL, do_call);

#undef DO_TOKEN
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
