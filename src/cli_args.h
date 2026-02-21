#ifndef ENDIAN_CLI_ARGS_H_
#define ENDIAN_CLI_ARGS_H_

struct cli_args {
  bool run_discord;
  bool default_root;
  bool verbose;
  char instance_dir[128];
};

void cli_args_parse(int argc, char** argv, struct cli_args* out);

#endif
