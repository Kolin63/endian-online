#ifndef ENDIAN_LOG_H_
#define ENDIAN_LOG_H_

#define LOGMOD_STATIC
#include <concord/logmod.h>

#include "bot.h"

#define log_log(level, ...)                                                    \
  logmod_log(level,                                                            \
             logmod_get_logger(                                                \
                 discord_get_logmod(bot_get_global()->discord_bot), "CLIENT"), \
             __VA_ARGS__);

#define log_trace(...) log_log(TRACE, __VA_ARGS__)
#define log_debug(...) log_log(DEBUG, __VA_ARGS__)
#define log_info(...) log_log(INFO, __VA_ARGS__)
#define log_warn(...) log_log(WARN, __VA_ARGS__)
#define log_error(...) log_log(ERROR, __VA_ARGS__)
#define log_fatal(...) log_log(FATAL, __VA_ARGS__)

#endif
