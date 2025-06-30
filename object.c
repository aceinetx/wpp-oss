#include "object.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
wppObject_free (wppObject *obj)
{
  if (obj->type == WPP_OBJ_ARRAY)
    {
      obj->type = WPP_OBJ_NULL;
      if (obj->as.array.array != NULL)
        {
          unsigned int i;
          for (i = 0; i < obj->as.array.length; i++)
            {
              if (obj->as.array.array != NULL)
                {
                  wppObject_free (&obj->as.array.array[i]);
                }
            }
          free (obj->as.array.array);
          obj->as.array.array = NULL;
          obj->as.array.length = 0;
          obj->as.array.size = 0;
        }
    }
}
