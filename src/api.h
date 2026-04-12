#ifndef ENDIAN_API_H_
#define ENDIAN_API_H_

#include <concord/discord.h>

#define API_VERSION 1

#ifndef ENDIAN_ENGINE

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define log_trace(api, ...) \
  api->log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(api, ...) \
  api->log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(api, ...) \
  api->log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(api, ...) \
  api->log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(api, ...) \
  api->log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(api, ...) \
  api->log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#endif

struct api {
  int version;
  void (*log_log)(int level, const char* file, int line, const char* fmt, ...);
  CCORDcode (*discord_create_interaction_response)(
      struct discord* client, u64snowflake interaction_id,
      const char interaction_token[],
      struct discord_interaction_response* params,
      struct discord_ret_interaction_response* ret);

  // puts a new registry on the heap. registry_cleanup() must be called when it
  // is done being used
  struct registry* (*registry_init)(int val_size);

  // frees allocated memory for a registry. if the registry contains structs
  // with data on the heap, those fields must be freed before calling this
  // function
  void (*registry_cleanup)(struct registry* reg);

  // adds a key and a value. returns -1 if the key already exists
  int (*registry_add)(struct registry* reg, const char* key, const void* val);

  // index to value. no bounds checking
  void* (*registry_itov)(const struct registry* reg, int i);

  // index to value. returns NULL on error
  void* (*registry_itov_safe)(const struct registry* reg, int i);

  // index to key. no bounds checking
  const char* (*registry_itok)(const struct registry* reg, int i);

  // index to key. returns NULL on error
  const char* (*registry_itok_safe)(const struct registry* reg, int i);

  // key to index. returns -1 if the key doesn't exist
  int (*registry_ktoi)(const struct registry* reg, const char* key);

  // key to value. returns NULL on error
  void* (*registry_ktov)(const struct registry* reg, const char* key);

  const struct registry* (*get_plugin_registry)();
  const struct registry* (*get_function_registry)();
  const struct registry* (*get_command_registry)();
};

#ifdef ENDIAN_ENGINE

void api_init();
void api_cleanup();

const struct api* api_get_global();

void api_distribute();

#endif

#endif
