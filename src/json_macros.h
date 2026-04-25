#ifndef ENDIAN_JSON_MACROS_H_
#define ENDIAN_JSON_MACROS_H_

// checks that a bool is the proper type
// assumes the following variables exist:
// - struct json_iterator* iter
// - const char* file_name
// - const char* mod_name
// - int error
#define END_JSON_CHECK_BOOL                                                                             \
  if (iter->json->type != cJSON_False && iter->json->type != cJSON_True) {                              \
    log_error("In data type %s from mod %s, %s must be bool", file_name, mod_name, iter->json->string); \
    error++;                                                                                            \
    continue;                                                                                           \
  }

// checks that a number is the proper type
// assumes the following variables exist:
// - struct json_iterator* iter
// - const char* file_name
// - const char* mod_name
// - int error
#define END_JSON_CHECK_NUMBER                                                                             \
  if (iter->json->type == cJSON_Number) {                                                                 \
    log_error("In data type %s from mod %s, %s must be number", file_name, mod_name, iter->json->string); \
    error++;                                                                                              \
    continue;                                                                                             \
  }

// checks that an int is the proper type and has no decimal places
// assumes the following variables exist:
// - struct json_iterator* iter
// - const char* file_name
// - const char* mod_name
// - int error
#define END_JSON_CHECK_INT                                                                                 \
  END_JSON_CHECK_NUMBER;                                                                                   \
  if ((int)iter->json->valuedouble != iter->json->valuedouble) {                                           \
    log_error("In data type %s from mod %s, %s must be integer", file_name, mod_name, iter->json->string); \
    error++;                                                                                               \
    continue;                                                                                              \
  }

// checks that a string is the proper type and is not null
// assumes the following variables exist:
// - struct json_iterator* iter
// - const char* file_name
// - const char* mod_name
// - int error
#define END_JSON_CHECK_STRING                                                                             \
  if (iter->json->type != cJSON_String) {                                                                 \
    log_error("In data type %s from mod %s, %s must be string", file_name, mod_name, iter->json->string); \
    error++;                                                                                              \
    continue;                                                                                             \
  }                                                                                                       \
  if (iter->json->valuestring == NULL) {                                                                  \
    log_error("In data type %s from mod %s, %s is NULL", file_name, mod_name, iter->json->string);        \
    error++;                                                                                              \
    continue;                                                                                             \
  }

// checks that string length is: min <= length <= max
// assumes the following variables exist:
// - struct json_iterator* iter
// - const char* file_name
// - const char* mod_name
// - int error
#define END_JSON_CHECK_STRING_LENGTH(min, max)                                                             \
  {                                                                                                        \
    int len = strlen(iter->json->valuestring);                                                             \
    if (len < min || len > max) {                                                                          \
      log_error("In data type %s from mod %s, %s is wrong size", file_name, mod_name, iter->json->string); \
      error++;                                                                                             \
      continue;                                                                                            \
    }                                                                                                      \
  }

// checks that an array is the proper type
// assumes the following variables exist:
// - struct json_iterator* iter
// - const char* file_name
// - const char* mod_name
// - int error
#define END_JSON_CHECK_ARRAY                                                                             \
  if (iter->json->type != cJSON_Array) {                                                                 \
    log_error("In data type %s from mod %s, %s must be array", file_name, mod_name, iter->json->string); \
    error++;                                                                                             \
    continue;                                                                                            \
  }

// checks that an array is the proper type and has at least one child
// assumes the following variables exist:
// - struct json_iterator* iter
// - const char* file_name
// - const char* mod_name
// - int error
#define END_JSON_CHECK_ARRAY_AND_CHILDREN                                                                  \
  END_JSON_CHECK_ARRAY;                                                                                    \
  if (iter->json->child == NULL) {                                                                         \
    log_error("In data type %s from mod %s, %s has no children", file_name, mod_name, iter->json->string); \
    error++;                                                                                               \
    continue;                                                                                              \
  }

// checks that the root object is an array and has at least one child
// assumes the following variables exist:
// - cJSON* json
// - const char* file_name
// - const char* mod_name
// - int error
#define END_JSON_CHECK_ROOT_ARRAY                                                                    \
  if (json->type != cJSON_Array) {                                                                   \
    log_error("In data type %s from mod %s, %s must be array", file_name, mod_name, json->string);   \
    error++;                                                                                         \
    return error;                                                                                    \
  }                                                                                                  \
  if (json->child == NULL) {                                                                         \
    log_error("In data type %s from mod %s, %s has no children", file_name, mod_name, json->string); \
    error++;                                                                                         \
    return error;                                                                                    \
  }

// checks that an object is the proper type
// assumes the following variables exist:
// - struct json_iterator* iter
// - const char* file_name
// - const char* mod_name
// - int error
#define END_JSON_CHECK_OBJECT                                                                             \
  if (iter->json->type != cJSON_Object) {                                                                 \
    log_error("In data type %s from mod %s, %s must be object", file_name, mod_name, iter->json->string); \
    error++;                                                                                              \
    continue;                                                                                             \
  }

// checks that an object is the proper type and has at least one child
// assumes the following variables exist:
// - struct json_iterator* iter
// - const char* file_name
// - const char* mod_name
// - int error
#define END_JSON_CHECK_OBJECT_AND_CHILDREN                                                                 \
  END_JSON_CHECK_OBJECT;                                                                                   \
  if (iter->json->child == NULL) {                                                                         \
    log_error("In data type %s from mod %s, %s has no children", file_name, mod_name, iter->json->string); \
    error++;                                                                                               \
    continue;                                                                                              \
  }

#endif
