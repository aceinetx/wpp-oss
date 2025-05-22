#include "exec.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool wpp_do_print (wppExec *exec);
bool wpp_do_println (wppExec *exec);
bool wpp_do_fn (wppExec *exec);
bool wpp_do_nf (wppExec *exec);
bool wpp_do_call (wppExec *exec);
bool wpp_do_var (wppExec *exec);
bool wpp_do_cp (wppExec *exec);
bool wpp_do_scanln (wppExec *exec);

wppExec *
wpp_exec_new (wppLexer *lexer)
{
  wppExec *exec = malloc (sizeof (wppExec));
  memset (exec, 0, sizeof (wppExec));

  exec->lexer = lexer;
  exec->objects_arena = wpp_arena_new ();
  exec->strings_arena = wpp_arena_new ();
  exec->ret_stack_top = -1;
  return exec;
}

void
wpp_exec_free (wppExec *exec)
{
  if (exec->vars)
    {
      free (exec->vars);
    }

  wpp_arena_free (&exec->objects_arena);
  wpp_arena_free (&exec->strings_arena);
  free (exec);
}

int
wpp_exec_fcall (wppExec *exec, const char *name)
{
  unsigned int i;
  for (i = 0; i < exec->vars_len; i++)
    {
      wppObject *obj = exec->vars[i];
      if (obj->type == WPP_OBJ_FUNCTION)
        {
          if (strcmp (obj->name, name) == 0)
            {
              if (wpp_exec_push_ret_stack (exec, exec->lexer->pos)
                  == PUSH_FAIL)
                {
                  return FCALL_FAIL;
                }
              exec->lexer->pos = obj->as.function.pos;
              return FCALL_SUCCESS;
            }
        }
    }
  return FCALL_FAIL;
}

void
wpp_exec_run (wppExec *exec)
{
  wppToken token;
  bool main_iteration = false;
  if (exec->lexer->pos > 0)
    main_iteration = true;

  token = wpp_lexer_next (exec->lexer);
  while (token.type != WPP_TOKEN_END)
    {
#define DO_TOKEN(t, f)                                                        \
  if (token.type == t)                                                        \
    if (!f (exec))                                                            \
      return;

      DO_TOKEN (WPP_TOKEN_FN, wpp_do_fn);
      DO_TOKEN (WPP_TOKEN_NF, wpp_do_nf);
      DO_TOKEN (WPP_TOKEN_VAR, wpp_do_var);
      if (main_iteration)
        {
          DO_TOKEN (WPP_TOKEN_PRINTLN, wpp_do_println);
          DO_TOKEN (WPP_TOKEN_PRINT, wpp_do_print);
          DO_TOKEN (WPP_TOKEN_CALL, wpp_do_call);
          DO_TOKEN (WPP_TOKEN_CP, wpp_do_cp);
          DO_TOKEN (WPP_TOKEN_SCANLN, wpp_do_scanln);
        }

#undef DO_TOKEN
      token = wpp_lexer_next (exec->lexer);
    }

  if (!main_iteration)
    {
      if (wpp_exec_fcall (exec, "main") == FCALL_FAIL)
        {
          sprintf (exec->error, "wpp_exec_fcall failed: no main function");
          return;
        }

      wpp_exec_run (exec);
    }
}
