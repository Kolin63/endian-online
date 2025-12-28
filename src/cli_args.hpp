#ifndef ENDIAN_CLI_ARGS_HPP_
#define ENDIAN_CLI_ARGS_HPP_

#include <string>

namespace end {

class CliArgs {
 public:
  CliArgs(int argc, char** argv);

  void PrintHelp();

  bool error{false};
  bool run_discord{true};
  bool default_root{false};
  std::string instance_dir{""};

 private:
  void MakeInstanceDirWithDefaultRoot();
};

}  // namespace end

#endif  // ENDIAN_CLI_ARGS_HPP_
