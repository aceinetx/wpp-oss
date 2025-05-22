#include "exec.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strdup (const char *);

wppObject *
wpp_exec_getvar (wppExec *exec, const char *name)
{
  unsigned int i;
  for (i = 0; i < exec->vars_len; i++)
    {
      wppObject *obj = exec->vars[i];
      if (strcmp (obj->name, name) == 0)
        return obj;
    }
  return NULL;
}

wppObject *
wpp_exec_assign (wppExec *exec, const char *name, wppObject object)
{
  unsigned int i;
  for (i = 0; i < exec->vars_len; i++)
    {
      wppObject *obj = exec->vars[i];
      if (strcmp (obj->name, name) == 0)
        {
          char *old_name = obj->name;
          memcpy (obj, &object, sizeof (wppObject));
          obj->name = old_name;
          return obj;
        }
    }

  /* didn't find a variable: create it */
  {
    wppObject *obj_alloc = malloc (sizeof (wppObject));
    memcpy (obj_alloc, &object, sizeof (wppObject));
    obj_alloc->name = strdup (name);

    wpp_arena_append (&exec->strings_arena, obj_alloc->name);
    wpp_arena_append (&exec->objects_arena, obj_alloc);

    exec->vars_capacity += sizeof (wppObject *);
    exec->vars = realloc (exec->vars, exec->vars_capacity);
    exec->vars[exec->vars_len] = obj_alloc;
    exec->vars_len++;
    return obj_alloc;
  }
}

bool
wpp_do_var (wppExec *exec)
{
  wppObject object;
  wppObject *var;
  wppToken name, eq, value;

  name = wpp_lexer_next (exec->lexer);
  eq = wpp_lexer_next (exec->lexer);
  value = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (name, WPP_TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (value, WPP_TOKEN_INT);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  object.type = WPP_OBJ_INT;
  object.as._int = value.as.number;
  object.name = name.as.str;

  var = wpp_exec_getvar (exec, name.as.str);
  if (!var)
    {
      wpp_exec_assign (exec, name.as.str, object);
    }

  switch (eq.type)
    {
    case WPP_TOKEN_EQ:
      if (var)
        if (!wpp_exec_obj_eq (exec, var, &object))
          return false;
      break;
    case WPP_TOKEN_ADD:
      if (!wpp_exec_obj_add (exec, var, &object))
        return false;
      break;
    case WPP_TOKEN_SUB:
      if (!wpp_exec_obj_sub (exec, var, &object))
        return false;
      break;
    case WPP_TOKEN_MUL:
      if (!wpp_exec_obj_mul (exec, var, &object))
        return false;
      break;
    case WPP_TOKEN_DIV:
      if (!wpp_exec_obj_div (exec, var, &object))
        return false;
      break;
    default:
      printf ("wpp: syntax error\n");
      return false;
    }

  return true;
}
