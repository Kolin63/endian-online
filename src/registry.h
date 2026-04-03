/*
   registry.h - part of the registry.c library by Colin Melican (Kolin63)

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

#ifndef COLIN_REGISTRY_H_
#define COLIN_REGISTRY_H_

// an interface to a single registry. none of these fields should be manually
// written to. to read from the keys or the values, either manually reading or
// using the functions are ok
struct registry {
  // amount of keys / values
  int length;

  // size of value type in bytes
  int val_size;

  // array of keys
  char** keys;

  // value data. continguous in memory
  void* values;
};

// puts a new registry on the heap. registry_cleanup() must be called when it
// is done being used
struct registry* registry_init(int val_size);

// frees allocated memory for a registry. if the registry contains structs with
// data on the heap, those fields must be freed before calling this function
void registry_cleanup(struct registry* reg);

// adds a key and a value. returns -1 if the key already exists
int registry_add(struct registry* reg, const char* key, const void* val);

// index to value. no bounds checking
void* registry_itov(struct registry* reg, int i);

// index to value. returns NULL on error
void* registry_itov_safe(struct registry* reg, int i);

// index to key. no bounds checking
char* registry_itok(struct registry* reg, int i);

// index to key. returns NULL on error
char* registry_itok_safe(struct registry* reg, int i);

// key to index. returns -1 if the key doesn't exist
int registry_ktoi(struct registry* reg, const char* key);

// key to value. returns NULL on error
void* registry_ktov(struct registry* reg, const char* key);

#endif
