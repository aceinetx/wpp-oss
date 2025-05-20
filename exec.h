#ifndef EXEC_H
#define EXEC_H
#include "lexer.h"
#include "object.h"

enum
{
  FCALL_FAIL,
  FCALL_SUCCESS
};

typedef struct
{
  char error[256];
  Lexer *lexer;

  Object *vars;
  unsigned int vars_len;
  unsigned int vars_capacity;

  Arena strings_arena;
} Exec;

Exec *exec_new (Lexer *lexer);
void exec_free (Exec *exec);

int exec_fcall (Exec *exec, const char *name);
void exec_run (Exec *exec);

#endif
