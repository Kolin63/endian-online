#include "plugin.h"

#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "registry.h"
#include "regman.h"
#include "sds.h"

void plugin_load(const char* plugin_path, const char* mod_name, const char* plugin_name) {
  if (strcmp(plugin_name, "src") == 0) return;

  dlerror();  // clear error
  void* dl = dlopen(plugin_path, RTLD_NOW | RTLD_GLOBAL);
  if (!dl) {
    log_error("Error loading plugin %s from mod %s: %s", plugin_name, mod_name, dlerror());
    return;
  }

  // without file extension
  sds clean_name = sdsnew(plugin_name);
  char* dot_finder = clean_name;
  while (*dot_finder != '\0' && *dot_finder != '.') dot_finder++;
  sdsrange(clean_name, 0, dot_finder - clean_name - 1);

  struct plugin plugin;
  plugin.plugin = dl;
  plugin.name = malloc(strlen(clean_name) + 1);
  strcpy(plugin.name, clean_name);

  if (registry_add(regman_get_plugin(), &plugin) == NULL) {
    log_error("Plugin %s already registered", plugin_name);
  }
  log_info("Loading plugin %s from mod %s", plugin_name, mod_name);

  sdsfree(clean_name);
}

int plugin_cmp(const void* a, const void* b) {
  const struct plugin* x = a;
  const struct plugin* y = b;
  return registry_strcmp(x->name, y->name);
}

void plugin_cleanup(void* elem) {
  struct plugin* plugin = elem;
  free(plugin->name);
  dlclose(plugin->plugin);
}
