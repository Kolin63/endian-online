#ifndef ENDIAN_JSON_MACROS_H_
#define ENDIAN_JSON_MACROS_H_

// checks that a string is the proper type and is not null
// assumes the following variables exist:
// - struct json_iterator* iter
// - const char* file_name
// - const char* mod_name
// - int error
#define END_JSON_CHECK_STRING                                                 \
  if (iter->json->type != cJSON_String) {                                     \
    log_error("In data type %s from mod %s, %s must be string", file_name,    \
              mod_name, iter->json->string);                                  \
    error++;                                                                  \
    continue;                                                                 \
  }                                                                           \
  if (iter->json->valuestring == NULL) {                                      \
    log_error("In data type %s from mod %s, %s is NULL", file_name, mod_name, \
              iter->json->string);                                            \
    error++;                                                                  \
    continue;                                                                 \
  }

// checks that string length is: min <= length <= max
// assumes the following variables exist:
// - struct json_iterator* iter
// - const char* file_name
// - const char* mod_name
// - int error
#define END_JSON_CHECK_STRING_LENGTH(min, max)                              \
  {                                                                         \
    int len = strlen(iter->json->valuestring);                              \
    if (len < min || len > max) {                                           \
      log_error("In data type %s from mod %s, %s is wrong size", file_name, \
                mod_name, iter->json->string);                              \
      error++;                                                              \
      continue;                                                             \
    }                                                                       \
  }

// checks that an array is the proper type and has at least one child
// assumes the following variables exist:
// - struct json_iterator* iter
// - const char* file_name
// - const char* mod_name
// - int error
#define END_JSON_CHECK_ARRAY                                                \
  if (iter->json->type != cJSON_Array) {                                    \
    log_error("In data type %s from mod %s, %s must be array", file_name,   \
              mod_name, iter->json->string);                                \
    error++;                                                                \
    continue;                                                               \
  }                                                                         \
  if (iter->json->child == NULL) {                                          \
    log_error("In data type %s from mod %s, %s has no children", file_name, \
              mod_name, iter->json->string);                                \
    error++;                                                                \
    continue;                                                               \
  }

#endif
