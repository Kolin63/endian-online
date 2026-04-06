#ifndef ENDIAN_FUNCTION_H_
#define ENDIAN_FUNCTION_H_

#include <concord/discord.h>

enum function_type {
  CALLBACK = 1,
  GET_API = 2,
};

struct function {
  enum function_type type;
  const void (*function)(...);
  char* name;
  char* plugin_name;
};

void function_load(const char* function_path, const char* mod_name,
                   const char* file_name);

#endif
