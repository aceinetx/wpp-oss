#include "exec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool
wpp_exec_obj_eq (wppExec *exec, wppObject *obj, wppObject *other)
{
  (void)exec;
  memcpy (obj, other, sizeof (wppObject));
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
