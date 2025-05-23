#include "exec.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strdup (const char *);

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
  wppObject arg_var;
  unsigned int arg_index;
  char arg_name[32];

  arg_index = 1;

  name = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (name, WPP_TOKEN_IDENTIFIER);

  for (;;)
    {
      wppToken tok;
      tok = wpp_lexer_next (exec->lexer);

      if (tok.type == WPP_TOKEN_SEMICOLON)
        break;

      sprintf (arg_name, "arg%u", arg_index);
      arg_var.name = strdup (arg_name);
      wpp_arena_append (&exec->strings_arena, arg_var.name);

      if (tok.type == WPP_TOKEN_INT)
        {
          arg_var.type = WPP_OBJ_INT;
          arg_var.as._int = tok.as.number;
        }
      else if (tok.type == WPP_TOKEN_FLOAT)
        {
          arg_var.type = WPP_OBJ_FLOAT;
          arg_var.as._float = tok.as.fnumber;
        }
      else if (tok.type == WPP_TOKEN_STRING)
        {
          arg_var.type = WPP_OBJ_STRING;
          arg_var.as.string = tok.as.str;
        }
      else if (tok.type == WPP_TOKEN_IDENTIFIER)
        {
          wppObject *var;
          var = wpp_exec_getvar (exec, tok.as.str);
          if (!var)
            {
              sprintf (exec->error, "call: undefined variable");
              return false;
            }

          wpp_exec_obj_eq (exec, &arg_var, var);
        }
      else
        {
          sprintf (exec->error, "call: invalid token");
          return false;
        }

      wpp_exec_assign (exec, arg_name, arg_var);

      arg_index++;
    }

  if (wpp_exec_fcall (exec, name.as.str) == FCALL_FAIL)
    return false;
  return true;
}
