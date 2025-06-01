#include "exec.h"
#include <stdio.h>
#include <string.h>

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
wpp_do_len (wppExec *exec)
{
  wppToken varname, strname;
  wppObject *str, var;

  varname = wpp_lexer_next (exec->lexer);
  strname = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (varname, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (strname, WPP_TOKEN_IDENTIFIER);

  str = wpp_exec_getvar (exec, strname.as.str);
  if (!str)
    {
      snprintf (exec->error, sizeof (exec->error),
                "len: undefined variable %s", strname.as.str);
      return false;
    }

  if (str->type != WPP_OBJ_STRING)
    {
      snprintf (exec->error, sizeof (exec->error), "len: %s is not a string",
                strname.as.str);
      return false;
    }

  var.type = WPP_OBJ_INT;
  var.as._int = strlen (str->as.string);

  wpp_exec_assign (exec, varname.as.str, var);

  return true;
}
