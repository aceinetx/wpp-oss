#ifndef ARENA_H
#define ARENA_H

#include <stdint.h>

typedef struct
{
  void **data;
  uint32_t size;
  uint32_t length;
} Arena;

Arena wpp_arena_new (void);

void *wpp_arena_alloc (Arena *arena, uint32_t size);
void wpp_arena_append (Arena *arena, void *ptr);

void wpp_arena_free (Arena *arena);

#endif
