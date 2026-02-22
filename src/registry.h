#ifndef ENDIAN_REGISTRY_H_
#define ENDIAN_REGISTRY_H_

#include <stdlib.h>

struct registry {
  const char* name;
  size_t length;
  size_t val_size;
  char** keys;
  void* values;
};

struct registry* registry_init(const char* name, size_t val_size);
void registry_cleanup(struct registry* reg);

void registry_add(struct registry* reg, const char* key, const void* val);

#endif
