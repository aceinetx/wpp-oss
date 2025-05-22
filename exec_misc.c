#include "exec.h"
#include <stdio.h>

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
          sprintf (exec->error, "wpp: undefined var");
          return false;
        }

      wpp_exec_obj_eq (exec, &obj, var);
      break;
    default:
      sprintf (exec->error, "wpp: syntax error");
      return false;
    }

  wpp_exec_assign (exec, "ret", obj);
  return true;
}
