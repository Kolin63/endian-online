/*
   registry.c - part of the registry.c library by Colin Melican (Kolin63)

   https://github.com/kolin63/registry.c

   Under the MIT License

   Copyright (c) 2026 Colin Melican

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
 */

#include "registry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct registry* registry_init(int val_size) {
  struct registry* reg = malloc(sizeof(struct registry));
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

int registry_add(struct registry* reg, const char* key, const void* val) {
  // do a binary search to find insertion index
  // 0 2 4 6 8
  //       ^
  //       5 (insert at index 3)
  int insert_index = 0;
  {
    int left = 0;
    int right = reg->length - 1;
    while (left <= right) {
      int mid = left + (right - left) / 2;
      int cmp = strcmp(reg->keys[mid], key);
      if (cmp < 0) {
        left = mid + 1;
      } else if (cmp > 0) {
        right = mid - 1;
      } else {
        fprintf(stderr, "Key already exists in registry: %s\n", key);
        return -1;
      }
    }
    insert_index = left + (right - left) / 2;
  }

  // move keys and values to make room
  // 0 2 4 _ 6 8
  //       ^
  //       5

  // first keys
  {
    reg->keys = realloc(reg->keys, (reg->length + 1) * sizeof(char*));
    char** src = reg->keys + insert_index;
    char** dest = src + 1;
    const size_t n = (reg->length - insert_index) * sizeof(char*);
    memmove(dest, src, n);
  }

  // then values
  {
    reg->values = realloc(reg->values, (reg->length + 1) * reg->val_size);
    void* src = reg->values + insert_index * reg->val_size;
    void* dest = src + reg->val_size;
    const size_t n = (reg->length - insert_index) * reg->val_size;
    memmove(dest, src, n);
  }

  // move in new key
  {
    char* new_key = malloc(strlen(key) + 1);
    strcpy(new_key, key);
    *(reg->keys + insert_index) = new_key;
  }

  // move in new value
  {
    const void* src = val;
    void* dest = reg->values + insert_index * reg->val_size;
    const size_t n = reg->val_size;
    memcpy(dest, src, n);
  }

  reg->length++;
  return 0;
}

void* registry_itov(struct registry* reg, int i) {
  return reg->values + i * reg->val_size;
}

void* registry_itov_safe(struct registry* reg, int i) {
  if (i < 0 || i >= reg->length) {
    return NULL;
  } else {
    return reg->values + i * reg->val_size;
  }
}

const char* registry_itok(struct registry* reg, int i) { return reg->keys[i]; }

const char* registry_itok_safe(struct registry* reg, int i) {
  if (i < 0 || i >= reg->length) {
    return NULL;
  } else {
    return reg->keys[i];
  }
}

int registry_ktoi(struct registry* reg, const char* key) {
  int left = 0;
  int right = reg->length - 1;
  while (left <= right) {
    int mid = left + (right - left) / 2;
    int cmp = strcmp(reg->keys[mid], key);
    if (cmp < 0) {
      left = mid + 1;
    } else if (cmp > 0) {
      right = mid - 1;
    } else {
      return mid;
    }
  }
  return -1;
}

void* registry_ktov(struct registry* reg, const char* key) {
  int i = registry_ktoi(reg, key);
  if (i < 0) {
    return NULL;
  } else {
    return registry_itov(reg, i);
  }
}
