#ifndef EXEC_H
#define EXEC_H
#include "lexer.h"
#include "object.h"

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

typedef struct
{
  char error[256];
  Lexer *lexer;

  Object **vars;
  unsigned int vars_len;
  unsigned int vars_capacity;

  Arena objects_arena;
  Arena strings_arena;
} Exec;

Exec *exec_new (Lexer *lexer);
void exec_free (Exec *exec);

int exec_fcall (Exec *exec, const char *name);
Object *exec_getvar (Exec *exec, const char *name);
Object *exec_assign (Exec *exec, const char *name, Object object);

void exec_run (Exec *exec);

#endif
