/*
   Made by Colin Melican (kolin63)
   MIT License
   https://github.com/kolin63/cjson-iterator
 */
#include "json_iterator.h"

#include <stdlib.h>
#include <assert.h>
#ifndef JSON_ITERATOR_INCLUDE_COMMENT
#include <string.h>
#endif

#include "cJSON.h"

// make sure the cjson passed is referencing the root object. remember to call
// json_iterator_cleanup() when you're done
struct json_iterator* json_iterator_init(const cJSON* json) {
  struct json_iterator* iter = malloc(sizeof(struct json_iterator));
  iter->json = json;
  iter->parent = NULL;
  return iter;
}

// only call once iterating is done
void json_iterator_cleanup(struct json_iterator* iter) {
  assert(iter->parent == NULL);
  free(iter);
}

// skip the object the iterator is currently on and all of it's children, if it
// has any
struct json_iterator* json_iterator_skip_object(struct json_iterator* iter) {
  if (iter->json != NULL && iter->json->next != NULL) {
    iter->json = iter->json->next;
  } else {
    iter->json = NULL;
  }
  return iter;
}

// iterate through cjson data
// should look like this:
/*
```c
cJSON* json = cJSON_Parse(buf);
struct json_iterator* iter = json_iterator_init(json);

while (true) {
  iter = json_iterate(iter);
  if (iter->parent == NULL) break;
  // ...
}
json_iterator_cleanup(iter);
  */
struct json_iterator* json_iterate(struct json_iterator* iter) {
  if (iter->json != NULL && iter->json->child != NULL) {
    struct json_iterator* child = malloc(sizeof(struct json_iterator));
    child->json = iter->json->child;
    child->parent = iter;
    iter = child;
  } else if (iter->json != NULL && iter->json->next != NULL) {
    iter->json = iter->json->next;
  } else if (iter->parent != NULL) {  // child and next are NULL
    struct json_iterator* dead_child = iter;
    iter = dead_child->parent;
    free(dead_child);
    iter = json_iterator_skip_object(iter);
  } else {  // child and next and parent are NULL
    iter->json = NULL;
  }

#ifndef JSON_ITERATOR_INCLUDE_COMMENT
  // completely ignore anything with the key "comment"
  if (iter->json != NULL && iter->json->string != NULL &&
      strlen(iter->json->string) == strlen("comment") &&
      strcmp(iter->json->string, "comment") == 0) {
    iter = json_iterator_skip_object(iter);
  }
#endif

  return iter;
}
