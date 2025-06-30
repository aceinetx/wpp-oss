#include "exec.h"
#include <stdlib.h>

enum
{
  CCALL_ARRAY_NEW = 1,
  CCALL_ARRAY_GET,
  CCALL_ARRAY_APPEND,
  CCALL_ARRAY_POP,
  CCALL_ARRAY_DELETE,
  CCALL_ARRAY_SET,
  CCALL_ARRAY_SIZE
};

bool
wpp_do_ccall (wppExec *exec)
{
  wppToken id_tok;
  int id;

  id_tok = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (id_tok, WPP_TOKEN_INT);

  id = id_tok.as.number;

#define EXPECT_VAR_TYPE(obj, t)                                               \
  if (obj->type != t)                                                         \
    {                                                                         \
      snprintf (exec->error, sizeof (exec->error),                            \
                "ccall: invalid type for %s", obj->name);                     \
      return false;                                                           \
    }
#define GETVAR(var, name)                                                     \
  if (!(var = wpp_exec_getvar (exec, name)))                                  \
    {                                                                         \
      snprintf (exec->error, sizeof (exec->error),                            \
                "ccall: undefined variable %s", name);                        \
      return false;                                                           \
    }
#define MEM_ERR()                                                             \
  {                                                                           \
    snprintf (exec->error, sizeof (exec->error), "ccall: memory error");      \
    return false;                                                             \
  }

  switch (id)
    {
    case CCALL_ARRAY_NEW:
      {
        wppObject *name, arr;
        GETVAR (name, "arg1");
        EXPECT_VAR_TYPE (name, WPP_OBJ_STRING);

        arr.type = WPP_OBJ_ARRAY;
        arr.as.array.length = 0;
        arr.as.array.size = 0;
        arr.as.array.array = NULL;
        wpp_exec_assign (exec, name->as.string, arr);
      }
      break;
    case CCALL_ARRAY_GET:
      {
        wppObject *name, *arr, *index;

        GETVAR (name, "arg1");
        EXPECT_VAR_TYPE (name, WPP_OBJ_STRING);

        GETVAR (arr, name->as.string);
        EXPECT_VAR_TYPE (arr, WPP_OBJ_ARRAY);

        GETVAR (index, "arg2");
        EXPECT_VAR_TYPE (index, WPP_OBJ_INT);

        if (index->as._int >= (int)arr->as.array.length || index->as._int < 0)
          {
            snprintf (exec->error, sizeof (exec->error),
                      "ccall: index %d is outside of bounds of an array with "
                      "length %d",
                      index->as._int, arr->as.array.length);
            return false;
          }

        wpp_exec_assign (exec, "ret", arr->as.array.array[index->as._int]);
      }
      break;
    case CCALL_ARRAY_APPEND:
      {
        wppObject *name, *arr, *obj;

        GETVAR (name, "arg1");
        EXPECT_VAR_TYPE (name, WPP_OBJ_STRING);

        GETVAR (arr, name->as.string);
        EXPECT_VAR_TYPE (arr, WPP_OBJ_ARRAY);

        GETVAR (obj, "arg2");

        arr->as.array.array = realloc (
            arr->as.array.array, arr->as.array.size + sizeof (wppObject));
        if (!arr->as.array.array)
          MEM_ERR ();

        arr->as.array.array[arr->as.array.length] = *obj;
        arr->as.array.length++;
        arr->as.array.size += sizeof (wppObject);
      }
      break;
    case CCALL_ARRAY_POP:
      {
        wppObject *name, *arr, obj;

        GETVAR (name, "arg1");
        EXPECT_VAR_TYPE (name, WPP_OBJ_STRING);

        GETVAR (arr, name->as.string);
        EXPECT_VAR_TYPE (arr, WPP_OBJ_ARRAY);

        if (arr->as.array.length == 0)
          {
            snprintf (exec->error, sizeof (exec->error),
                      "ccall: pop from empty array");
            return false;
          }

        obj = arr->as.array.array[arr->as.array.length - 1];
        if (arr->as.array.length == 1)
          {
            free (arr->as.array.array);
            arr->as.array.array = NULL;
          }
        else
          {
            arr->as.array.array = realloc (
                arr->as.array.array, arr->as.array.size - sizeof (wppObject));
          }
        arr->as.array.length--;
        arr->as.array.size -= sizeof (wppObject);

        wpp_exec_assign (exec, "ret", obj);
      }
      break;
    case CCALL_ARRAY_DELETE:
      {
        wppObject *name, *arr, *index;
        unsigned int i;

        GETVAR (name, "arg1");
        EXPECT_VAR_TYPE (name, WPP_OBJ_STRING);

        GETVAR (arr, name->as.string);
        EXPECT_VAR_TYPE (arr, WPP_OBJ_ARRAY);

        GETVAR (index, "arg2");
        EXPECT_VAR_TYPE (index, WPP_OBJ_INT);

        if (index->as._int >= (int)arr->as.array.length || index->as._int < 0)
          {
            snprintf (exec->error, sizeof (exec->error),
                      "ccall: index %d is outside of bounds of an array with "
                      "length %d",
                      index->as._int, arr->as.array.length);
            return false;
          }

        for (i = index->as._int; i < arr->as.array.length - 1; i++)
          {
            (arr->as.array.array)[i] = (arr->as.array.array)[i + 1];
          }

        if (arr->as.array.length == 1)
          {
            free (arr->as.array.array);
            arr->as.array.array = NULL;
          }
        else
          {
            arr->as.array.array = realloc (
                arr->as.array.array, arr->as.array.size - sizeof (wppObject));
          }

        arr->as.array.size -= sizeof (wppObject);
        arr->as.array.length--;
      }
      break;
    case CCALL_ARRAY_SET:
      {
        wppObject *name, *arr, *index, *obj;

        GETVAR (name, "arg1");
        EXPECT_VAR_TYPE (name, WPP_OBJ_STRING);

        GETVAR (arr, name->as.string);
        EXPECT_VAR_TYPE (arr, WPP_OBJ_ARRAY);

        GETVAR (index, "arg2");
        EXPECT_VAR_TYPE (index, WPP_OBJ_INT);

        GETVAR (obj, "arg3");

        if (index->as._int >= (int)arr->as.array.length || index->as._int < 0)
          {
            snprintf (exec->error, sizeof (exec->error),
                      "ccall: index %d is outside of bounds of an array with "
                      "length %d",
                      index->as._int, arr->as.array.length);
            return false;
          }

        arr->as.array.array[index->as._int] = *obj;
      }
      break;
    case CCALL_ARRAY_SIZE:
      {
        wppObject *name, *arr, obj;

        GETVAR (name, "arg1");
        EXPECT_VAR_TYPE (name, WPP_OBJ_STRING);

        GETVAR (arr, name->as.string);
        EXPECT_VAR_TYPE (arr, WPP_OBJ_ARRAY);

        obj.type = WPP_OBJ_INT;
        obj.as._int = arr->as.array.length;

        wpp_exec_assign (exec, "ret", obj);
      }
      break;
    default:
      snprintf (exec->error, sizeof (exec->error),
                "ccall: unknown ccall index: %d", id);
      return false;
    }
  return true;
}
