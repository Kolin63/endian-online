#ifndef ENDIAN_BOT_HPP_
#define ENDIAN_BOT_HPP_

#include <dpp/dpp.h>

#include <string>
#include <string_view>

namespace end {

class Bot {
 public:
  Bot() = delete;
  explicit Bot(std::string instance_dir);
  Bot(std::string instance_dir, bool run_discord);

  // loads the bot token from <instance_dir>/token.txt
  // returns true on success, false on failure
  bool LoadToken();
  std::string_view GetToken();

 private:
  // makes sure that the instance directory is properly set up
  // returns true on success, false on failure
  bool BuildInstanceDir();

  std::string instance_dir_{};
  bool run_discord_{true};
  std::string token_{};

  dpp::cluster discord_bot_;
};

}  // namespace end

#endif  // ENDIAN_BOT_HPP_
