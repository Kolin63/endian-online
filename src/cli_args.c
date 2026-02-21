#include "cli_args.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(const char* program_name) {
  printf("Usage: %s [-drv] <instance_name>\n", program_name);
}

void cli_args_parse(int argc, char** argv, struct cli_args* out) {
  out->run_discord = true;
  out->default_root = true;
  out->verbose = false;
  strcpy(out->instance_dir, "");

  int req_args_passed = 0;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {  // a flag
      for (size_t j = 1; j < strlen(argv[i]); j++) {
        switch (argv[i][j]) {
        case 'd':
          out->run_discord = false;
          break;
        case 'r':
          out->default_root = false;
          break;
        case 'v':
          out->verbose = true;
          break;
        }
      }
    } else {  // not a flag; the instance name
      strcpy(out->instance_dir, argv[i]);
      req_args_passed++;
    }
  }  // done parsing args

  if (req_args_passed != 1) {
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  if (out->default_root == true) {
    char buf[128];
    strcpy(buf, out->instance_dir);
#ifdef __linux__
    strcpy(out->instance_dir, getenv("HOME"));
    strcat(out->instance_dir, "/.local/share/endian/");
#else
    static_assert(0, "Default root not supported on this OS");
#endif
    strcat(out->instance_dir, buf);
  }
}
