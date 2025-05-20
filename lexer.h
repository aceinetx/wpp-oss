#ifndef LEXER_H
#define LEXER_H

#include "arena.h"
#include <stdint.h>

typedef struct
{
  enum
  {
    TOKEN_NULL = 0,
    TOKEN_END,

    TOKEN_IDENTIFIER,
    TOKEN_STRING,

    TOKEN_FN,
    TOKEN_NF,

    TOKEN_PRINT,
    TOKEN_PRINTLN,

    TOKEN_CALL,

    TOKEN_SEMICOLON
  } type;

  union
  {
    int number;
    char *str;
  } as;

  unsigned int line;
} Token;

typedef struct
{
  char *code;
  unsigned int pos;

  Arena strings_arena;

  unsigned int line;
} Lexer;

Lexer *lexer_new (char *code);
void lexer_free (Lexer *lexer);

Token lexer_next (Lexer *lexer);

#endif
