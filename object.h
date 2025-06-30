#ifndef OBJECT_H
#define OBJECT_H

enum
{
  WPP_OBJ_NULL,
  WPP_OBJ_INT,
  WPP_OBJ_FLOAT,
  WPP_OBJ_STRING,
  WPP_OBJ_FUNCTION,
  WPP_OBJ_ARRAY
};

typedef struct wppObject
{
  unsigned char type;

  union
  {
    struct
    {
      unsigned int pos;
    } function;

    struct
    {
      unsigned int size;
      unsigned int length;
      struct wppObject *array;
    } array;

    int _int;
    float _float;
    char *string;
  } as;

  char *name;
} wppObject;

void wppObject_free (wppObject *obj);

#endif
