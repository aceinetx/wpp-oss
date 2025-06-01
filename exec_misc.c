#include "exec.h"
#include <stdio.h>
#include <stdlib.h>
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
wpp_do_getchar (wppExec *exec)
{
  wppToken strname, index, varname;
  wppObject res, *str;
  int _index;

  strname = wpp_lexer_next (exec->lexer);
  index = wpp_lexer_next (exec->lexer);
  varname = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (strname, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (varname, WPP_TOKEN_IDENTIFIER);

  if (index.type == WPP_TOKEN_INT)
    {
      _index = index.as.number;
    }
  else if (index.type == WPP_TOKEN_IDENTIFIER)
    {
      wppObject *var;
      var = wpp_exec_getvar (exec, index.as.str);
      if (!var)
        {
          snprintf (exec->error, sizeof (exec->error),
                    "getchar: undefined variable %s", index.as.str);
          return false;
        }
      if (var->type != WPP_OBJ_INT)
        {
          snprintf (exec->error, sizeof (exec->error),
                    "getchar: index variable is not an integer");
          return false;
        }
      _index = var->as._int;
    }
  else
    {
      snprintf (exec->error, sizeof (exec->error), "getchar: syntax error");
      return false;
    }

  str = wpp_exec_getvar (exec, strname.as.str);
  if (!str)
    {
      snprintf (exec->error, sizeof (exec->error),
                "getchar: undefined variable %s", strname.as.str);
      return false;
    }

  if (_index >= (int)strlen (str->as.string))
    {
      snprintf (exec->error, sizeof (exec->error),
                "getchar: index %d is out of bounds of a string of length %d",
                _index, (int)strlen (str->as.string));
      return false;
    }

  res.type = WPP_OBJ_STRING;
  res.as.string = malloc (2);
  res.as.string[0] = str->as.string[_index];
  res.as.string[1] = 0;

  wpp_arena_append (&exec->strings_arena, res.as.string);
  wpp_exec_assign (exec, varname.as.str, res);

  return true;
}
