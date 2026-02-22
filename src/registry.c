#include "registry.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"

struct registry* registry_init(const char* name, size_t val_size) {
  struct registry* reg = malloc(sizeof(struct registry));
  reg->name = name;
  reg->length = 0;
  reg->val_size = val_size;
  reg->keys = NULL;
  reg->values = NULL;
  return reg;
}

void registry_cleanup(struct registry* reg) {
  for (size_t i = 0; i < reg->length; i++) {
    free(reg->keys[i]);
  }
  free(reg->keys);
  free(reg->values);
  free(reg);
}

void registry_add(struct registry* reg, const char* key, const void* val) {
  // first, we need to find what index to insert at
  size_t insert_index = 0;
  if (reg->length > 0) {
    size_t left = 0;
    size_t right = reg->length - 1;
    size_t mid = left + (right - left) / 2;
    while (left <= right) {
      const int cmp = strcmp(reg->keys[mid], key);
      if (cmp < 0) {
        left = mid + 1;
      } else if (cmp > 0) {
        if (mid == 0) {
          goto integer_overflow_fix;
        }
        right = mid - 1;
      } else {
        log_error("Registry %s already has key %s", reg->name, key);
        return;
      }
    }
  integer_overflow_fix:
    insert_index = mid;
  }

  // then, we realloc and add the value
  if (reg->length == 0) {
    reg->length++;
    reg->keys = malloc(sizeof(char*));
    reg->values = malloc(reg->val_size);
  } else {
    reg->length++;
    reg->keys = realloc(reg->keys, reg->length * sizeof(char*));
    reg->values = realloc(reg->values, reg->length * reg->val_size);

    // shift keys and values starting from insert_index (inclusive) one to the
    // right to make room for new insertion
    memmove(reg->keys + insert_index + 1, reg->keys + insert_index,
            (reg->length - insert_index - 1) * sizeof(char*));
    memmove((char*)reg->values + (insert_index + 1) * reg->val_size,
            reg->values, (reg->length - insert_index - 1) * reg->val_size);
  }

  // move in the new key and value
  reg->keys[insert_index] = malloc(strlen(key) + 1);
  strcpy(reg->keys[insert_index], key);
  memcpy(((char*)reg->values) + insert_index * reg->val_size, val,
         reg->val_size);

  log_info("Added to registry %s key: %s", reg->name, key);
}
