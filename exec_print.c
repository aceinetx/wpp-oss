#include "exec.h"
#include "wpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
wpp_print_object (wppObject *var, bool raw)
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
      if (raw)
        printf ("\"%s\"", var->as.string);
      else
        printf ("%s", var->as.string);
      break;
    case WPP_OBJ_FUNCTION:
      printf ("<function %s at %p>", var->name, (void *)var);
      break;
    case WPP_OBJ_HASHMAP:
      {
        unsigned int i;
        /*
  printf ("<hashmap %s (%p) of len %u>", var->name,
  (void *)var->as.hashmap.data, var->as.hashmap.length);
                                        */
        printf ("{");
        for (i = 0; i < var->as.hashmap.length; i++)
          {
            wpp_print_object (&var->as.hashmap.data[i], true);
            if (i < var->as.hashmap.length - 1)
              {
                printf (", ");
              }
          }
        printf ("}");
      }
      break;
    case WPP_OBJ_HASHMAP_ENTRY:
      {
        printf ("%s: ", var->as.hashmap_entry.key);
        wpp_print_object (var->as.hashmap_entry.obj, true);
      }
      break;
    case WPP_OBJ_ARRAY:
      {
        unsigned int i;
        /*
  printf ("<array %s (%p) of len %u>", var->name,
  (void *)var->as.array.array, var->as.array.length);
                                        */
        printf ("{");
        for (i = 0; i < var->as.array.length; i++)
          {
            wpp_print_object (&var->as.array.data[i], true);
            if (i < var->as.array.length - 1)
              {
                printf (", ");
              }
          }
        printf ("}");
      }
      break;
    default:
      printf ("<%s of type %d>", var->name, var->type);
      break;
    }
}

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
              if (var->type == WPP_OBJ_ARRAY)
                wpp_print_object (var, true);
              else
                wpp_print_object (var, false);
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
