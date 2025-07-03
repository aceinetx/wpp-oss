#include "exec.h"
#include <string.h>

bool
wpp_do_len (wppExec *exec)
{
  wppToken varname, strname;
  wppObject var, *str;

  varname = wpp_lexer_next (exec->lexer);
  strname = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (varname, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (strname, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

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

bool
wpp_do_getchar (wppExec *exec)
{
  wppToken strname, index, varname;
  wppObject *str, var;
  uint32_t idx;

  strname = wpp_lexer_next (exec->lexer);
  index = wpp_lexer_next (exec->lexer);
  varname = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (strname, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (varname, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  str = wpp_exec_getvar (exec, strname.as.str);
  if (!str)
    {
      snprintf (exec->error, sizeof (exec->error),
                "getchar: undefined variable %s", strname.as.str);
      return false;
    }

  if (str->type != WPP_OBJ_STRING)
    {
      snprintf (exec->error, sizeof (exec->error),
                "getchar: %s is not a string", strname.as.str);
      return false;
    }

  switch (index.type)
    {
    case WPP_TOKEN_INT:
      idx = (uint32_t)index.as.number;
      break;
    case WPP_TOKEN_IDENTIFIER:
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
                      "getchar: %s is not an integer", index.as.str);
            return false;
          }
        idx = (uint32_t)var->as._int;
      }
      break;
    default:
      snprintf (exec->error, sizeof (exec->error), "getchar: syntax error");
      return false;
    }

  if (idx >= strlen (str->as.string))
    {
      snprintf (exec->error, sizeof (exec->error),
                "getchar: index %d is outside of string bounds of length %u",
                idx, (uint32_t)strlen (str->as.string));
      return false;
    }

  var.type = WPP_OBJ_STRING;
  var.as.string = wpp_arena_alloc (&exec->strings_arena, 2);
  var.as.string[0] = str->as.string[idx];
  var.as.string[1] = '\0';

  wpp_exec_assign (exec, varname.as.str, var);

  return true;
}

bool
wpp_do_setchar (wppExec *exec)
{
  wppToken strname, chr_tok, index;
  wppObject *str;
  char chr;
  uint32_t idx;

  strname = wpp_lexer_next (exec->lexer);
  chr_tok = wpp_lexer_next (exec->lexer);
  index = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (strname, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  str = wpp_exec_getvar (exec, strname.as.str);
  if (!str)
    {
      snprintf (exec->error, sizeof (exec->error),
                "setchar: undefined variable %s", strname.as.str);
      return false;
    }

  if (str->type != WPP_OBJ_STRING)
    {
      snprintf (exec->error, sizeof (exec->error),
                "setchar: %s is not a string", strname.as.str);
      return false;
    }

  switch (chr_tok.type)
    {
    case WPP_TOKEN_STRING:
      chr = chr_tok.as.str[0];
      break;
    case WPP_TOKEN_IDENTIFIER:
      {
        wppObject *var;
        var = wpp_exec_getvar (exec, chr_tok.as.str);
        if (!var)
          {
            snprintf (exec->error, sizeof (exec->error),
                      "setchar: undefined variable %s", chr_tok.as.str);
            return false;
          }
        if (var->type != WPP_OBJ_STRING)
          {
            snprintf (exec->error, sizeof (exec->error),
                      "setchar: %s is not a string", chr_tok.as.str);
            return false;
          }
        chr = var->as.string[0];
      }
      break;
    default:
      snprintf (exec->error, sizeof (exec->error), "setchar: syntax error");
      return false;
    }

  switch (index.type)
    {
    case WPP_TOKEN_INT:
      idx = (uint32_t)index.as.number;
      break;
    case WPP_TOKEN_IDENTIFIER:
      {
        wppObject *var;
        var = wpp_exec_getvar (exec, index.as.str);
        if (!var)
          {
            snprintf (exec->error, sizeof (exec->error),
                      "setchar: undefined variable %s", index.as.str);
            return false;
          }
        if (var->type != WPP_OBJ_INT)
          {
            snprintf (exec->error, sizeof (exec->error),
                      "setchar: %s is not an integer", index.as.str);
            return false;
          }
        idx = (uint32_t)var->as._int;
      }
      break;
    default:
      snprintf (exec->error, sizeof (exec->error), "setchar: syntax error");
      return false;
    }

  if (idx >= strlen (str->as.string))
    {
      snprintf (exec->error, sizeof (exec->error),
                "setchar: index %d is outside of string bounds of length %u",
                idx, (uint32_t)strlen (str->as.string));
      return false;
    }

  str->as.string[idx] = chr;

  return true;
}
