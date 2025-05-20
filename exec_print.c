#include "exec.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool
do_println (Exec *exec)
{
  Token string;
  char *c;
  char *varname;
  char *varname_c;

  string = lexer_next (exec->lexer);
  DO_TEST_TOKEN (string, TOKEN_STRING);

  varname = malloc (strlen (string.as.str));

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
                case OBJ_FUNCTION:
                  printf ("<function %s at %p:%d>", varname, (void *)var,
                          var->as.function.pos);
                  break;
                default:
                  printf ("<unhandled var type>");
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

  putchar ('\n');
  free (varname);

  return true;
}
