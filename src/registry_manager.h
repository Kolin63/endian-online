#ifndef ENDIAN_REGISTRY_MANAGER_H_
#define ENDIAN_REGISTRY_MANAGER_H_

#include "registry.h"

struct registry_manager {
  struct registry* command_registry;
};

void registry_manager_init();
void registry_manager_cleanup();

struct registry_manager* registry_manager_get_global();

struct registry* registry_manager_get_command_registry();

#endif
