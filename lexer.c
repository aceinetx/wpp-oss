#include "lexer.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool
is_letter (char c)
{
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

static bool
is_identifier_letter (char c)
{
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

static bool
is_digit (char c)
{
  return (c >= '0' && c <= '9');
}

/*
static bool
is_symbol (char c)
{
  return (c == '.' || c == ',' || c == '!' || c == ' ' || c == '$'
          || c == '^');
}
*/

static Token
token_new (void)
{
  Token token;
  token.line = 0;
  token.as.str = NULL;
  token.type = TOKEN_NULL;
  return token;
}

Lexer *
lexer_new (char *code)
{
  Lexer *lexer = malloc (sizeof (Lexer));
  lexer->strings_arena = arena_new ();
  lexer->code = code;
  lexer->line = 1;
  lexer->pos = 0;
  return lexer;
}

void
lexer_free (Lexer *lexer)
{
  arena_free (&lexer->strings_arena);
  free (lexer);
}

/* this doesn't append to strings arena: you'll have to do it manually */
static Token
lexer_identifier (Lexer *lexer)
{
  Token tok;
  size_t len;

  tok = token_new ();
  tok.type = TOKEN_IDENTIFIER;
  tok.line = lexer->line;
  tok.as.str = malloc (1);

  len = 1;
  while (lexer->pos < strlen (lexer->code))
    {
      char c = lexer->code[lexer->pos];

      if (!is_identifier_letter (c) && !is_digit (c))
        {
          break;
        }

      tok.as.str = realloc (tok.as.str, len + 1);
      tok.as.str[len - 1] = c;
      tok.as.str[len] = '\0';
      len++;

      lexer->pos++;
    }

  return tok;
}

static Token
lexer_string (Lexer *lexer)
{
  Token tok;
  size_t len;
  bool definition;

  tok = token_new ();
  tok.type = TOKEN_STRING;
  tok.line = lexer->line;
  tok.as.str = malloc (1);

  len = 1;

  definition = false;
  while (lexer->pos < strlen (lexer->code))
    {
      char c = lexer->code[lexer->pos];

      if (c == '"')
        {
          if (definition)
            {
              lexer->pos++;
              break;
            }
          definition = true;
          lexer->pos++;
          continue;
        }

      if (definition)
        {
          tok.as.str = realloc (tok.as.str, len + 1);
          tok.as.str[len - 1] = c;
          tok.as.str[len] = '\0';
          len++;
        }

      lexer->pos++;
    }

  arena_append (&lexer->strings_arena, tok.as.str);

  return tok;
}

static Token
lexer_number (Lexer *lexer)
{
  Token tok;
  bool is_hex;

  tok = token_new ();
  tok.type = TOKEN_INT;
  tok.line = lexer->line;

  is_hex = false;

  while (lexer->pos < strlen (lexer->code))
    {
      char c = lexer->code[lexer->pos];
      if (!is_digit (c) && !is_hex)
        {
          if (c == 'x')
            {
              is_hex = true;
              tok.as.number = 0;
              lexer->pos++;
              continue;
            }
          else
            {
              break;
            }
        }

      if (is_hex)
        {
          bool invalid;

          invalid = false;
          tok.as.number *= 16;

          if (is_digit (c))
            {
              tok.as.number += c - '0';
            }
          else
            {
              switch (c)
                {
                case 'a':
                  tok.as.number += 0xa;
                  break;
                case 'b':
                  tok.as.number += 0xb;
                  break;
                case 'c':
                  tok.as.number += 0xc;
                  break;
                case 'd':
                  tok.as.number += 0xd;
                  break;
                case 'e':
                  tok.as.number += 0xe;
                  break;
                case 'f':
                  tok.as.number += 0xf;
                  break;
                default:
                  invalid = true;
                  break;
                }
            }
          if (invalid)
            {
              tok.as.number /= 16;
              break;
            }
        }
      else
        {
          tok.as.number *= 10;

          tok.as.number += c - '0';
        }
      lexer->pos++;
    }

  return tok;
}

Token
lexer_next (Lexer *lexer)
{
  Token token = token_new ();

  while (lexer->pos < strlen (lexer->code))
    {
      char c = lexer->code[lexer->pos];
      token = token_new ();

      if (c == '\n')
        lexer->line++;

      if (is_letter (c))
        {
          token = lexer_identifier (lexer);
#define DO_IDENTIFIR(s, v)                                                    \
  if (strcmp (token.as.str, s) == 0)                                          \
    token.type = v;
          DO_IDENTIFIR ("fn", TOKEN_FN);
          DO_IDENTIFIR ("nf", TOKEN_NF);
          DO_IDENTIFIR ("print", TOKEN_PRINT);
          DO_IDENTIFIR ("println", TOKEN_PRINTLN);
          DO_IDENTIFIR ("call", TOKEN_CALL);
          DO_IDENTIFIR ("var", TOKEN_VAR);
          DO_IDENTIFIR ("cp", TOKEN_CP);
          DO_IDENTIFIR ("scanln", TOKEN_SCANLN);
#undef DO_IDENTIFIR
          if (token.type == TOKEN_IDENTIFIER)
            arena_append (&lexer->strings_arena, token.as.str);
          else
            free (token.as.str);
        }
      else if (is_digit (c))
        {
          token = lexer_number (lexer);
        }
      else if (c == '"')
        {
          token = lexer_string (lexer);
        }
      else if (c == ';')
        {
          token.type = TOKEN_SEMICOLON;
          lexer->pos++;
        }
      else if (c == '=')
        {
          token.type = TOKEN_EQ;
          lexer->pos++;
        }
      else if (c == '+')
        {
          token.type = TOKEN_ADD;
          lexer->pos++;
        }
      else if (c == '-')
        {
          token.type = TOKEN_SUB;
          lexer->pos++;
        }
      else if (c == '*')
        {
          token.type = TOKEN_MUL;
          lexer->pos++;
        }
      else if (c == '/')
        {
          token.type = TOKEN_DIV;
          lexer->pos++;
        }
      else if (c == '?')
        {
          /* comment */
          while (!(lexer->code[lexer->pos] == '-'
                   && lexer->code[lexer->pos + 1] == '?'))
            lexer->pos++;
          lexer->pos++;
        }

      if (token.type != TOKEN_NULL)
        {
          return token;
        }

      lexer->pos++;
    }

  token.type = TOKEN_END;
  token.line = lexer->line;
  return token;
}
