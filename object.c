#include "object.h"
#include <stddef.h>
#include <stdlib.h>

void
wppObject_free (wppObject *obj)
{
  if (obj->type == WPP_OBJ_ARRAY)
    {
      if (obj->as.array.array != NULL)
        free (obj->as.array.array);
    }
}
