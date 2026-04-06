#include "cli_args.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "sds.h"

void print_usage(const char* program_name) {
  printf("Usage: %s [-rv] <instance_name>\n", program_name);
}

struct cli_args* cli_args_init() {
  struct cli_args* args = malloc(sizeof(struct cli_args));
  args->instance_dir = sdsempty();
  return args;
}

void cli_args_cleanup(struct cli_args* args) {
  sdsfree(args->instance_dir);
  free(args);
}

void cli_args_parse(int argc, const char** argv, struct cli_args* out) {
  out->default_root = true;
  out->verbose = false;

  int req_args_passed = 0;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {  // a flag
      for (size_t j = 1; j < strlen(argv[i]); j++) {
        switch (argv[i][j]) {
        case 'r':
          out->default_root = false;
          break;
        case 'v':
          out->verbose = true;
          break;
        }
      }
    } else {  // not a flag; the instance name
      sdscpy(out->instance_dir, argv[i]);
      req_args_passed++;
    }
  }  // done parsing args

  if (req_args_passed != 1) {
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  if (out->default_root == true) {
    sds buf = sdsnew(out->instance_dir);
#ifdef __linux__
    const char* home = getenv("HOME");
    if (!home) {
      log_error("Error: could not get value of $HOME");
      exit(EXIT_FAILURE);
    }
    out->instance_dir = sdscpy(out->instance_dir, home);
    out->instance_dir = sdscat(out->instance_dir, "/.local/share/endian/");
#else
    static_assert(0, "Default root not supported on this OS");
#endif
    out->instance_dir = sdscat(out->instance_dir, buf);
    sdsfree(buf);
  }
}
