#include "cli_args.hpp"

#include <iostream>
#include <string>

end::CliArgs::CliArgs(int argc, char** argv) {
  for (int i{1}; i < argc; ++i) {
    std::string arg{argv[i]};
    if (arg[0] == '-' && arg.length() > 1 && arg[1] != '-') {
      for (std::size_t j{1}; j < arg.length(); ++j) {
        if (arg[j] == 'r') {
          default_root = true;
        } else if (arg[j] == 'd') {
          run_discord = false;
        } else {
          std::cerr << "Unknown flag -" << arg[j] << '\n';
          error = true;
          return;
        }
      }
    } else if (arg == "--help" || arg == "-h") {
      PrintHelp();
      error = true;
      return;
    } else if (arg[0] == '-') {
      std::cerr << "Unknown argument " << arg << '\n';
      error = true;
      return;
    } else {
      instance_dir = arg;
    }
  }

  if (instance_dir == "") {
    error = true;
    return;
  }

  if (default_root) {
    MakeInstanceDirWithDefaultRoot();
  }
}

void end::CliArgs::PrintHelp() {
  std::cout << R"(SYNOPSIS
    server <instance directory> [-rd]

OPTIONS
    -r    use Endian's default directory for root
    -d    disable the Discord bot
)";
}

void end::CliArgs::MakeInstanceDirWithDefaultRoot() {
  if (default_root) {
#ifdef __linux__
    const std::string HOME{std::getenv("HOME")};
    instance_dir = HOME + "/.local/share/endian/" + instance_dir;
#else
    static_assert("Operating system does not have a default root defined" &&
                  false);
#endif
  }
}
