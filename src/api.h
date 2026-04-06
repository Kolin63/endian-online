#ifndef ENDIAN_API_H_
#define ENDIAN_API_H_

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
};

#ifdef ENDIAN_ENGINE

void api_init();
void api_cleanup();

const struct api* api_get_global();

void api_distribute();

#endif

#endif
