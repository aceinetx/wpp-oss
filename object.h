#ifndef OBJECT_H
#define OBJECT_H

enum
{
  OBJ_INT,
  OBJ_FLOAT,
  OBJ_STRING,
  OBJ_FUNCTION
};

typedef struct
{
  unsigned char type;

  union
  {
    struct
    {
      unsigned int pos;
    } function;

    int _int;
    float _float;
    char *string;
  } as;

  char *name;
} Object;

#endif
