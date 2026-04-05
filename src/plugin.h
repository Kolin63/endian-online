#ifndef ENDIAN_PLUGIN_H_
#define ENDIAN_PLUGIN_H_

struct plugin {
  void* plugin;
  char* name;
};

void plugin_load(const char* plugin_path, const char* mod_name,
                 const char* plugin_name);

#endif
