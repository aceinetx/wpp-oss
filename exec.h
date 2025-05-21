#ifndef EXEC_H
#define EXEC_H
#include "lexer.h"
#include "object.h"
#include "wpp.h"
#include <stddef.h>

#define DO_TEST_TOKEN(v, t)                                                   \
  if (v.type != t)                                                            \
    {                                                                         \
      sprintf (exec->error, "syntax error");                                  \
      return false;                                                           \
    }

enum
{
  FCALL_FAIL,
  FCALL_SUCCESS
};

enum
{
  PUSH_FAIL,
  PUSH_SUCCESS
};

typedef struct
{
  char error[256];
  Object **vars;
  unsigned int vars_len;
  unsigned int vars_capacity;

  Arena objects_arena;
  Arena strings_arena;

  Lexer *lexer;

  unsigned int ret_stack[256];
  size_t ret_stack_top;
} Exec;

Exec *exec_new (Lexer *lexer);
void exec_free (Exec *exec);

int exec_fcall (Exec *exec, const char *name);
Object *exec_getvar (Exec *exec, const char *name);
Object *exec_assign (Exec *exec, const char *name, Object object);

int exec_push_ret_stack (Exec *exec, unsigned int value);
unsigned int exec_pop_ret_stack (Exec *exec);

void exec_run (Exec *exec);

bool exec_obj_eq (Exec *exec, Object *obj, Object *other);
bool exec_obj_add (Exec *exec, Object *obj, Object *other);
bool exec_obj_sub (Exec *exec, Object *obj, Object *other);
bool exec_obj_mul (Exec *exec, Object *obj, Object *other);
bool exec_obj_div (Exec *exec, Object *obj, Object *other);

#endif
