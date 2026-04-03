/*
   Made by Colin Melican (kolin63)
   MIT License
   https://github.com/kolin63/cjson-iterator
 */
#ifndef COLIN_JSON_ITERATOR_H_
#define COLIN_JSON_ITERATOR_H_

#include "cJSON.h"

// data structure used for iterating through cjson data
struct json_iterator {
  const cJSON* json;
  struct json_iterator* parent;
};

// make sure the cjson passed is referencing the root object. remember to call
// json_iterator_cleanup() when you're done
struct json_iterator* json_iterator_init(const cJSON* json);

// only call once iterating is done
void json_iterator_cleanup(struct json_iterator* iter);

// skip the object the iterator is currently on and all of it's children, if it
// has any
struct json_iterator* json_iterator_skip_object(struct json_iterator* iter);

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
cJSON_Delete(json);
  */
struct json_iterator* json_iterate(struct json_iterator* iter);

#endif
