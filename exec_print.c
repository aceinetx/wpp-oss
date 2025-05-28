#include "exec.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool
wpp_do_print (wppExec *exec)
{
  wppToken string;
  char *c, *varname, *varname_c;

  varname = NULL;

  string = wpp_lexer_next (exec->lexer);
  DO_TEST_TOKEN (string, WPP_TOKEN_STRING);
  DO_TEST_TOKEN (wpp_lexer_next (exec->lexer), WPP_TOKEN_SEMICOLON);

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
          wppObject *var;
          c++;

          varname_c = varname;
          memset (varname, 0, strlen (string.as.str));
          while (*c != '^' && *c)
            {
              *varname_c = *c;
              varname_c++;
              c++;
            }

          var = wpp_exec_getvar (exec, varname);
          if (var)
            {
              switch (var->type)
                {
                case WPP_OBJ_INT:
                  printf ("%d", var->as._int);
                  break;
                case WPP_OBJ_FLOAT:
                  printf ("%f", var->as._float);
                  break;
                case WPP_OBJ_STRING:
                  printf ("%s", var->as.string);
                  break;
                case WPP_OBJ_FUNCTION:
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
wpp_do_println (wppExec *exec)
{
  wpp_do_print (exec);

  putchar ('\n');
  return true;
}
