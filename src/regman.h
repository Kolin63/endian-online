#ifndef ENDIAN_REGMAN_H_
#define ENDIAN_REGMAN_H_

#include "registry.h"

struct regman {
  struct registry* plugin_registry;
  struct registry* function_registry;
  struct registry* command_registry;
  struct registry* user_registry;
};

void regman_init();
void regman_cleanup();

struct regman* regman_get();

struct registry* regman_get_plugin();
struct registry* regman_get_function();
struct registry* regman_get_command();

#endif
