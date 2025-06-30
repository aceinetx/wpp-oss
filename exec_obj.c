#include "exec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *
memdup (const void *src, size_t size)
{
  void *dest = malloc (size);
  memcpy (dest, src, size);
  return dest;
}

bool
wpp_exec_obj_eq (wppExec *exec, wppObject *obj, wppObject *other)
{
  char *name;
  (void)exec;
  name = obj->name;
  memcpy (obj, other, sizeof (wppObject));
  obj->name = name;
  if (other->type == WPP_OBJ_ARRAY)
    {
      unsigned int i;
      obj->as.array.array
          = memdup (other->as.array.array, other->as.array.size);
      for (i = 0; i < other->as.array.length; i++)
        {
          if (other->as.array.array[i].type == WPP_OBJ_ARRAY)
            {
              wpp_exec_obj_eq (exec, &obj->as.array.array[i],
                               &other->as.array.array[i]);
            }
        }
    }
  return true;
}

bool
wpp_exec_obj_add (wppExec *exec, wppObject *obj, wppObject *other)
{
  if (obj->type == WPP_OBJ_INT)
    {
      if (other->type == WPP_OBJ_INT)
        obj->as._int += other->as._int;
      else if (other->type == WPP_OBJ_FLOAT)
        obj->as._int += (int)other->as._float;
      else
        snprintf (exec->error, sizeof (exec->error), "unsupported operation");
    }
  else if (obj->type == WPP_OBJ_FLOAT)
    {
      if (other->type == WPP_OBJ_INT)
        obj->as._float += (float)other->as._int;
      else if (other->type == WPP_OBJ_FLOAT)
        obj->as._float += other->as._float;
      else
        snprintf (exec->error, sizeof (exec->error), "unsupported operation");
    }
  else if (obj->type == WPP_OBJ_STRING && other->type == WPP_OBJ_STRING)
    {
      size_t size;
      char *new;

      size = strlen (obj->as.string) + strlen (other->as.string) + 1;
      new = malloc (size);

      snprintf (new, size, "%s%s", obj->as.string, other->as.string);

      wpp_arena_append (&exec->strings_arena, new);

      obj->as.string = new;

      /* TODO: implement a function in wppArena to free the old string right
       there and not waste space */
    }
  else
    snprintf (exec->error, sizeof (exec->error), "unsupported operation");

  return !(*exec->error);
}

bool
wpp_exec_obj_sub (wppExec *exec, wppObject *obj, wppObject *other)
{
  if (obj->type == WPP_OBJ_INT)
    {
      if (other->type == WPP_OBJ_INT)
        obj->as._int -= other->as._int;
      else if (other->type == WPP_OBJ_FLOAT)
        obj->as._int -= (int)other->as._float;
      else
        snprintf (exec->error, sizeof (exec->error), "unsupported operation");
    }
  else if (obj->type == WPP_OBJ_FLOAT)
    {
      if (other->type == WPP_OBJ_INT)
        obj->as._float -= (float)other->as._int;
      else if (other->type == WPP_OBJ_FLOAT)
        obj->as._float -= other->as._float;
      else
        snprintf (exec->error, sizeof (exec->error), "unsupported operation");
    }
  else
    snprintf (exec->error, sizeof (exec->error), "unsupported operation");

  return !(*exec->error);
}

bool
wpp_exec_obj_mul (wppExec *exec, wppObject *obj, wppObject *other)
{
  if (obj->type == WPP_OBJ_INT)
    {
      if (other->type == WPP_OBJ_INT)
        obj->as._int *= other->as._int;
      else if (other->type == WPP_OBJ_FLOAT)
        obj->as._int *= (int)other->as._float;
      else
        snprintf (exec->error, sizeof (exec->error), "unsupported operation");
    }
  else if (obj->type == WPP_OBJ_FLOAT)
    {
      if (other->type == WPP_OBJ_INT)
        obj->as._float *= (float)other->as._int;
      else if (other->type == WPP_OBJ_FLOAT)
        obj->as._float *= other->as._float;
      else
        snprintf (exec->error, sizeof (exec->error), "unsupported operation");
    }
  else
    snprintf (exec->error, sizeof (exec->error), "unsupported operation");

  return !(*exec->error);
}

bool
wpp_exec_obj_div (wppExec *exec, wppObject *obj, wppObject *other)
{
  if (obj->type == WPP_OBJ_INT)
    {
      if (other->type == WPP_OBJ_INT)
        obj->as._int /= other->as._int;
      else if (other->type == WPP_OBJ_FLOAT)
        obj->as._int /= (int)other->as._float;
      else
        snprintf (exec->error, sizeof (exec->error), "unsupported operation");
    }
  else if (obj->type == WPP_OBJ_FLOAT)
    {
      if (other->type == WPP_OBJ_INT)
        obj->as._float /= (float)other->as._int;
      else if (other->type == WPP_OBJ_FLOAT)
        obj->as._float /= other->as._float;
      else
        snprintf (exec->error, sizeof (exec->error), "unsupported operation");
    }
  else
    snprintf (exec->error, sizeof (exec->error), "unsupported operation");

  return !(*exec->error);
}
