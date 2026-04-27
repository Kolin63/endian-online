#include "cli_args.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "sds.h"

static struct cli_args* global = NULL;

void print_usage(const char* program_name) {
  printf("Usage: %s [-rv] <instance_name>\n", program_name);
}

void cli_args_init() {
  global = malloc(sizeof(struct cli_args));
  global->instance_dir = sdsempty();
}

void cli_args_cleanup() {
  sdsfree(global->instance_dir);
  free(global);
}

void cli_args_parse(int argc, const char** argv) {
  global->default_root = true;
  global->verbose = false;

  int req_args_passed = 0;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {  // a flag
      for (size_t j = 1; j < strlen(argv[i]); j++) {
        switch (argv[i][j]) {
        case 'r':
          global->default_root = false;
          break;
        case 'v':
          global->verbose = true;
          break;
        }
      }
    } else {  // not a flag; the instance name
      global->instance_dir = sdscpy(global->instance_dir, argv[i]);
      req_args_passed++;
    }
  }  // done parsing args

  if (req_args_passed != 1) {
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  if (global->default_root == true) {
    sds buf = sdsnew(global->instance_dir);
#ifdef __linux__
    const char* home = getenv("HOME");
    if (!home) {
      log_error("Error: could not get value of $HOME");
      exit(EXIT_FAILURE);
    }
    global->instance_dir = sdscpy(global->instance_dir, home);
    global->instance_dir = sdscat(global->instance_dir, "/.local/share/endian/");
#else
    static_assert(0, "Default root not supported on this OS");
#endif
    global->instance_dir = sdscat(global->instance_dir, buf);
    sdsfree(buf);
  }
}

const struct cli_args* cli_args_get_global() { return global; }
