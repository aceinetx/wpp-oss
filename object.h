#ifndef OBJECT_H
#define OBJECT_H

typedef struct
{
  enum
  {
    OBJ_INT,
    OBJ_FLOAT,
    OBJ_STRING,
    OBJ_FUNCTION
  } type;

  union
  {
    struct
    {
      unsigned int pos;
      char *name;
    } function;

    int _int;
    float _float;
    char *string;
  } as;

  char *name;
} Object;

#endif
