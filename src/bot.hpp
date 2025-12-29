#ifndef ENDIAN_BOT_HPP_
#define ENDIAN_BOT_HPP_

#include <dpp/cluster.h>

#include <string>

namespace end {

class Bot {
 public:
  Bot() = delete;
  explicit Bot(std::string instance_dir);
  Bot(std::string instance_dir, bool run_discord);

  const std::string& GetInstanceDir() const;

  bool GetRunDiscord() const;
  dpp::cluster& GetDiscordBot();
  const dpp::cluster& GetDiscordBot() const;

  // loads the bot token from <instance_dir>/token.txt
  // returns true on success, false on failure
  bool LoadToken();
  const std::string& GetToken();
  const std::string& GetToken() const;

  static inline Bot* global_bot{nullptr};

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
