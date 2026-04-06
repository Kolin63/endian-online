#include "api.h"

#include <stdlib.h>

#include "function.h"
#include "log.h"
#include "registry.h"
#include "regman.h"

static struct api* global;

void api_init() {
  global = malloc(sizeof(struct api));
  global->version = API_VERSION;
  global->log_log = log_log;
}

void api_cleanup() { free(global); }

const struct api* api_get_global() { return global; }

// hands out api to all mod functions of type GET_API
void api_distribute() {
  const struct registry* func_reg = regman_get_function();
  for (int i = 0; i < func_reg->length; i++) {
    const struct function* func = registry_itov(func_reg, i);
    if (func->type != GET_API) continue;
    func->function(global);
  }
}
