#include "exec.h"
#include <stdio.h>
#include <string.h>

bool
exec_obj_eq (Exec *exec, Object *obj, Object *other)
{
  (void)exec;
  memcpy (obj, other, sizeof (Object));
  return true;
}

bool
exec_obj_add (Exec *exec, Object *obj, Object *other)
{
  if (obj->type == OBJ_INT)
    {
      if (other->type == OBJ_INT)
        obj->as._int += other->as._int;
      else if (other->type == OBJ_FLOAT)
        obj->as._int += (int)other->as._float;
      else
        sprintf (exec->error, "unsupported operation");
    }
  else if (obj->type == OBJ_FLOAT)
    {
      if (other->type == OBJ_INT)
        obj->as._float += (float)other->as._int;
      else if (other->type == OBJ_FLOAT)
        obj->as._float += other->as._float;
      else
        sprintf (exec->error, "unsupported operation");
    }
  else
    sprintf (exec->error, "unsupported operation");

  return !(*exec->error);
}

bool
exec_obj_sub (Exec *exec, Object *obj, Object *other)
{
  if (obj->type == OBJ_INT)
    {
      if (other->type == OBJ_INT)
        obj->as._int -= other->as._int;
      else if (other->type == OBJ_FLOAT)
        obj->as._int -= (int)other->as._float;
      else
        sprintf (exec->error, "unsupported operation");
    }
  else if (obj->type == OBJ_FLOAT)
    {
      if (other->type == OBJ_INT)
        obj->as._float -= (float)other->as._int;
      else if (other->type == OBJ_FLOAT)
        obj->as._float -= other->as._float;
      else
        sprintf (exec->error, "unsupported operation");
    }
  else
    sprintf (exec->error, "unsupported operation");

  return !(*exec->error);
}

bool
exec_obj_mul (Exec *exec, Object *obj, Object *other)
{
  if (obj->type == OBJ_INT)
    {
      if (other->type == OBJ_INT)
        obj->as._int *= other->as._int;
      else if (other->type == OBJ_FLOAT)
        obj->as._int *= (int)other->as._float;
      else
        sprintf (exec->error, "unsupported operation");
    }
  else if (obj->type == OBJ_FLOAT)
    {
      if (other->type == OBJ_INT)
        obj->as._float *= (float)other->as._int;
      else if (other->type == OBJ_FLOAT)
        obj->as._float *= other->as._float;
      else
        sprintf (exec->error, "unsupported operation");
    }
  else
    sprintf (exec->error, "unsupported operation");

  return !(*exec->error);
}

bool
exec_obj_div (Exec *exec, Object *obj, Object *other)
{
  if (obj->type == OBJ_INT)
    {
      if (other->type == OBJ_INT)
        obj->as._int /= other->as._int;
      else if (other->type == OBJ_FLOAT)
        obj->as._int /= (int)other->as._float;
      else
        sprintf (exec->error, "unsupported operation");
    }
  else if (obj->type == OBJ_FLOAT)
    {
      if (other->type == OBJ_INT)
        obj->as._float /= (float)other->as._int;
      else if (other->type == OBJ_FLOAT)
        obj->as._float /= other->as._float;
      else
        sprintf (exec->error, "unsupported operation");
    }
  else
    sprintf (exec->error, "unsupported operation");

  return !(*exec->error);
}
