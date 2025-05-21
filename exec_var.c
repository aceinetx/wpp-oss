#include "exec.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strdup (const char *);

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

Object *
exec_assign (Exec *exec, const char *name, Object object)
{
  unsigned int i;
  for (i = 0; i < exec->vars_len; i++)
    {
      Object *obj = exec->vars[i];
      if (strcmp (obj->name, name) == 0)
        {
          memcpy (obj, &object, sizeof (Object));
          return obj;
        }
    }

  /* didn't find a variable: create it */
  {
    Object *obj_alloc = malloc (sizeof (Object));
    memcpy (obj_alloc, &object, sizeof (Object));
    obj_alloc->name = strdup (name);

    arena_append (&exec->strings_arena, obj_alloc->name);
    arena_append (&exec->objects_arena, obj_alloc);

    exec->vars_capacity += sizeof (Object *);
    exec->vars = realloc (exec->vars, exec->vars_capacity);
    exec->vars[exec->vars_len] = obj_alloc;
    exec->vars_len++;
    return obj_alloc;
  }
}

bool
do_var (Exec *exec)
{
  Object object;
  Object *var;
  Token name, eq, value;

  name = lexer_next (exec->lexer);
  eq = lexer_next (exec->lexer);
  value = lexer_next (exec->lexer);
  DO_TEST_TOKEN (name, TOKEN_IDENTIFIER);
  DO_TEST_TOKEN (value, TOKEN_INT);
  DO_TEST_TOKEN (lexer_next (exec->lexer), TOKEN_SEMICOLON);

  object.type = OBJ_INT;
  object.as._int = value.as.number;
  object.name = name.as.str;

  var = exec_getvar (exec, name.as.str);
  if (!var)
    {
      exec_assign (exec, name.as.str, object);
    }

  switch (eq.type)
    {
    case TOKEN_EQ:
      if (var)
        if (!exec_obj_eq (exec, var, &object))
          return false;
      break;
    case TOKEN_ADD:
      if (!exec_obj_add (exec, var, &object))
        return false;
      break;
    case TOKEN_SUB:
      if (!exec_obj_sub (exec, var, &object))
        return false;
      break;
    case TOKEN_MUL:
      if (!exec_obj_mul (exec, var, &object))
        return false;
      break;
    case TOKEN_DIV:
      if (!exec_obj_div (exec, var, &object))
        return false;
      break;
    default:
      printf ("wpp: syntax error\n");
      return false;
    }

  return true;
}
