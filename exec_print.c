#include "exec.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool
do_print (Exec *exec)
{
  Token string;
  char *c;
  char *varname;
  char *varname_c;

  varname = NULL;

  string = lexer_next (exec->lexer);
  DO_TEST_TOKEN (string, TOKEN_STRING);
  DO_TEST_TOKEN (lexer_next (exec->lexer), TOKEN_SEMICOLON);

  /* do we even need to allocate varname? */
  c = string.as.str;
  while (*c)
    {
      if (*c == '$')
        {
          varname = malloc (strlen (string.as.str));
          break;
        }
      c++;
    }

  c = string.as.str;
  while (*c)
    {
      if (*c == '$')
        { /* print variable */
          Object *var;
          c++;

          varname_c = varname;
          memset (varname, 0, strlen (string.as.str));
          while (*c != '^' && *c)
            {
              *varname_c = *c;
              varname_c++;
              c++;
            }

          var = exec_getvar (exec, varname);
          if (var)
            {
              switch (var->type)
                {
                case OBJ_INT:
                  printf ("%d", var->as._int);
                  break;
                case OBJ_FLOAT:
                  printf ("%f", var->as._float);
                  break;
                case OBJ_STRING:
                  printf ("%s", var->as.string);
                  break;
                case OBJ_FUNCTION:
                  printf ("<function %s at %p>", varname, (void *)var);
                  break;
                default:
                  printf ("<%s of type %d>", varname, var->type);
                  break;
                }
            }
          else
            {
              printf ("<unknown var: %s>", varname);
            }
        }
      else
        /* print normal character */
        {
          putchar (*c);
        }
      c++;
    }

  if (varname)
    free (varname);

  return true;
}

bool
do_println (Exec *exec)
{
  do_print (exec);

  putchar ('\n');
  return true;
}
