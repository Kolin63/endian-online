#ifndef ENDIAN_CLI_ARGS_H_
#define ENDIAN_CLI_ARGS_H_

#include "sds.h"

struct cli_args {
  bool default_root;
  bool verbose;
  sds instance_dir;
};

void cli_args_init();
void cli_args_cleanup();

void cli_args_parse(int argc, const char** argv);

const struct cli_args* cli_args_get_global();

#endif
