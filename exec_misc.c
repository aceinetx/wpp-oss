#include "exec.h"
#include <stdio.h>
#include <time.h>

int usleep (__useconds_t __useconds);

bool wpp_do_nf (wppExec *exec);

bool
wpp_do_return (wppExec *exec)
{
  wppToken arg;
  wppObject obj, *var;

  arg = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  switch (arg.type)
    {
    case WPP_TOKEN_INT:
      obj.type = WPP_OBJ_INT;
      obj.as._int = arg.as.number;
      break;
    case WPP_TOKEN_FLOAT:
      obj.type = WPP_OBJ_FLOAT;
      obj.as._float = arg.as.fnumber;
      break;
    case WPP_TOKEN_IDENTIFIER:
      var = wpp_exec_getvar (exec, arg.as.str);
      if (!var)
        {
          snprintf (exec->error, sizeof (exec->error),
                    "return: undefined variable %s", arg.as.str);
          return false;
        }

      wpp_exec_obj_eq (exec, &obj, var);
      break;
    default:
      snprintf (exec->error, sizeof (exec->error), "return: syntax error");
      return false;
    }

  wpp_exec_assign (exec, "ret", obj);
  wpp_do_nf (exec);
  return true;
}

bool
wpp_do_exit (wppExec *exec)
{
  wppToken code_tok;
  wppObject *ret;
  int code = 0;
  code_tok = wpp_lexer_next (exec->lexer);

  switch (code_tok.type)
    {
    case WPP_TOKEN_IDENTIFIER:
      {
        wppObject *var;
        var = wpp_exec_getvar (exec, code_tok.as.str);
        if (!var)
          {
            snprintf (exec->error, sizeof (exec->error),
                      "exit: undefined variable %s", code_tok.as.str);
            return false;
          }

        if (var->type != WPP_OBJ_INT)
          {
            snprintf (exec->error, sizeof (exec->error),
                      "exit: %s is not an integer", code_tok.as.str);
            return false;
          }

        code = var->as._int;
        break;
      }
    case WPP_TOKEN_INT:
      code = code_tok.as.number;
      break;
    default:
      snprintf (exec->error, sizeof (exec->error), "exit: syntax error");
      return false;
    }

  ret = wpp_exec_getvar (exec, "ret");
  ret->as._int = code;
  return false;
}

bool
wpp_do_sleep (wppExec *exec)
{
  wppToken secs_tok;
  float seconds;
  secs_tok = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  switch (secs_tok.type)
    {
    case WPP_TOKEN_IDENTIFIER:
      {
        wppObject *var;
        var = wpp_exec_getvar (exec, secs_tok.as.str);
        if (!var)
          {
            snprintf (exec->error, sizeof (exec->error),
                      "exit: undefined variable %s", secs_tok.as.str);
            return false;
          }

        if (var->type != WPP_OBJ_INT && var->type != WPP_OBJ_FLOAT)
          {
            snprintf (exec->error, sizeof (exec->error),
                      "exit: %s is not an integer or a float",
                      secs_tok.as.str);
            return false;
          }

        if (var->type == WPP_OBJ_INT)
          seconds = (float)var->as._int;
        if (var->type == WPP_OBJ_FLOAT)
          seconds = var->as._float;
        break;
      }
    case WPP_TOKEN_INT:
      seconds = (float)secs_tok.as.number;
      break;
    case WPP_TOKEN_FLOAT:
      seconds = secs_tok.as.fnumber;
      break;
    default:
      snprintf (exec->error, sizeof (exec->error), "exit: syntax error");
      return false;
    }

  usleep (seconds * 1000000);

  return true;
}

bool
wpp_do_utime (wppExec *exec)
{
  wppToken varname;
  wppObject var;

  varname = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (varname, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  var.type = WPP_OBJ_INT;
  var.as._int = (int)time (NULL);

  wpp_exec_assign (exec, varname.as.str, var);

  return true;
}
