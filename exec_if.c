#include "exec.h"
#include <stdio.h>
#include <string.h>

bool
wpp_do_if (wppExec *exec)
{
  wppToken lhs_tok, op_tok, rhs_tok;
  wppObject lhs, rhs;
  bool skip;

  skip = true;

  lhs_tok = wpp_lexer_next (exec->lexer);
  op_tok = wpp_lexer_next (exec->lexer);
  rhs_tok = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

  lhs.type = (unsigned char)-1;
  rhs.type = (unsigned char)-1;

  if (lhs_tok.type == WPP_TOKEN_IDENTIFIER)
    {
      wppObject *var = wpp_exec_getvar (exec, lhs_tok.as.str);
      if (!var)
        {
          snprintf (exec->error, sizeof (exec->error),
                    "if: undefined variable %s", lhs_tok.as.str);
          return false;
        }
      memcpy (&lhs, var, sizeof (wppObject));
    }
  if (rhs_tok.type == WPP_TOKEN_IDENTIFIER)
    {
      wppObject *var = wpp_exec_getvar (exec, rhs_tok.as.str);
      if (!var)
        {
          snprintf (exec->error, sizeof (exec->error),
                    "if: undefined variable %s", rhs_tok.as.str);
          return false;
        }
      memcpy (&rhs, var, sizeof (wppObject));
    }

  if (lhs_tok.type == WPP_TOKEN_INT)
    {
      lhs.type = WPP_OBJ_INT;
      lhs.as._int = lhs_tok.as.number;
    }
  if (rhs_tok.type == WPP_TOKEN_INT)
    {
      rhs.type = WPP_OBJ_INT;
      rhs.as._int = rhs_tok.as.number;
    }

  if (lhs_tok.type == WPP_TOKEN_FLOAT)
    {
      lhs.type = WPP_OBJ_FLOAT;
      lhs.as._float = lhs_tok.as.fnumber;
    }
  if (rhs_tok.type == WPP_TOKEN_FLOAT)
    {
      rhs.type = WPP_OBJ_FLOAT;
      rhs.as._float = rhs_tok.as.fnumber;
    }

  if (lhs.type == (unsigned char)-1)
    {
      snprintf (exec->error, sizeof (exec->error),
                "if: unsupported type for lhs operand");
      return false;
    }
  if (rhs.type == (unsigned char)-1)
    {
      snprintf (exec->error, sizeof (exec->error),
                "if: unsupported type for rhs operand");
      return false;
    }

    /* perform comparisons */
#define DO_CMP(oper)                                                          \
  if (lhs.type == WPP_TOKEN_INT)                                              \
    {                                                                         \
      if (rhs.type == WPP_TOKEN_INT)                                          \
        skip = !(lhs.as._int oper rhs.as._int);                               \
      if (rhs.type == WPP_TOKEN_FLOAT)                                        \
        skip = !(lhs.as._int oper (int) rhs.as._float);                       \
    }                                                                         \
  if (lhs.type == WPP_TOKEN_FLOAT)                                            \
    {                                                                         \
      if (rhs.type == WPP_TOKEN_INT)                                          \
        skip = !(lhs.as._float oper (float) rhs.as._int);                     \
      if (rhs.type == WPP_TOKEN_FLOAT)                                        \
        skip = !(lhs.as._float oper rhs.as._float);                           \
    }

  switch (op_tok.type)
    {
    case WPP_TOKEN_EQEQ:
      skip = !memcmp (&lhs.as, &rhs.as, sizeof (lhs.as));
      break;
    case WPP_TOKEN_NOEQ:
      skip = memcmp (&lhs.as, &rhs.as, sizeof (lhs.as));
      break;
    case WPP_TOKEN_LESS:
      DO_CMP (<);
      break;
    case WPP_TOKEN_MORE:
      DO_CMP (>);
      break;
    case WPP_TOKEN_LSEQ:
      DO_CMP (<=);
      break;
    case WPP_TOKEN_MREQ:
      DO_CMP (>=);
      break;
    default:
      snprintf (exec->error, sizeof (exec->error),
                "if: invalid token of type %d", op_tok.type);
      return false;
    }
#undef DO_CMP

  if (skip)
    {
      for (;;)
        {
          wppToken token = wpp_lexer_next (exec->lexer);
        }
    }

  return true;
}
