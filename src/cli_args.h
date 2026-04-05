#ifndef ENDIAN_CLI_ARGS_H_
#define ENDIAN_CLI_ARGS_H_

#include "sds.h"

struct cli_args {
  bool default_root;
  bool verbose;
  sds instance_dir;
};

struct cli_args* cli_args_init();
void cli_args_cleanup(struct cli_args* args);

void cli_args_parse(int argc, const char** argv, struct cli_args* out);

#endif
