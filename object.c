#include "object.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
wppObject_free (wppObject *obj)
{
  if (obj->type == WPP_OBJ_HASHMAP)
    {
      obj->type = WPP_OBJ_NULL;
      if (obj->as.hashmap.data != NULL)
        {
          unsigned int i;
          for (i = 0; i < obj->as.hashmap.length; i++)
            {
              if (obj->as.hashmap.data != NULL)
                {
                  wppObject_free (&obj->as.hashmap.data[i]);
                }
            }
          free (obj->as.hashmap.data);
          obj->as.hashmap.data = NULL;
          obj->as.hashmap.length = 0;
          obj->as.hashmap.size = 0;
        }
    }
  else if (obj->type == WPP_OBJ_HASHMAP_ENTRY)
    {
      obj->type = WPP_OBJ_NULL;
      if (obj->as.hashmap_entry.obj)
        {
          wppObject_free (obj->as.hashmap_entry.obj);
          free (obj->as.hashmap_entry.obj);
          obj->as.hashmap_entry.obj = NULL;
        }
    }
  else if (obj->type == WPP_OBJ_ARRAY)
    {
      obj->type = WPP_OBJ_NULL;
      if (obj->as.array.data != NULL)
        {
          unsigned int i;
          for (i = 0; i < obj->as.array.length; i++)
            {
              if (obj->as.array.data != NULL)
                {
                  wppObject_free (&obj->as.array.data[i]);
                }
            }
          free (obj->as.array.data);
          obj->as.array.data = NULL;
          obj->as.array.length = 0;
          obj->as.array.size = 0;
        }
    }
}
