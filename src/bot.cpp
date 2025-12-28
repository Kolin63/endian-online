#include "bot.hpp"

#include <filesystem>
#include <fstream>
#include <string>

#include "log.hpp"

end::Bot::Bot(std::string instance_dir) : end::Bot{instance_dir, true} {}

end::Bot::Bot(std::string instance_dir, bool run_discord)
    : instance_dir_{instance_dir},
      run_discord_{run_discord},
      discord_bot_{std::string{GetToken()}} {
  BuildInstanceDir();

  if (run_discord_) {
    discord_bot_.on_log(end::Log::DppLogger());

    discord_bot_.on_slashcommand([](const dpp::slashcommand_t& event) {
      if (event.command.get_command_name() == "ping") {
        event.reply("Pong!");
      }
    });

    discord_bot_.on_ready(
        [&discord_bot_ = discord_bot_](const dpp::ready_t& event) {
          if (dpp::run_once<struct register_bot_commands>()) {
            discord_bot_.global_command_create(
                dpp::slashcommand("ping", "Ping pong!", discord_bot_.me.id));
          }
        });

    discord_bot_.start(dpp::st_wait);
  }
}

bool end::Bot::LoadToken() {
  const std::string token_location{instance_dir_ + "/token.txt"};
  if (!std::filesystem::exists(token_location)) {
    Log::Critical("Token Location " + token_location + " does not exist.");
    return false;
  }

  std::ifstream fs{};
  fs.open(token_location);

  fs >> token_;

  if (fs.fail() || token_ == "") {
    Log::Print("There was a problem with trying to read from the token file.");
    return false;
  }

  Log::Print({Message::info, "Token succesfully read."});
  return true;
}

std::string_view end::Bot::GetToken() {
  if (token_ == "") LoadToken();
  return token_;
}

bool end::Bot::BuildInstanceDir() {
  // don't cry, it's just a harmless little macro
#define mkdir_helper(path)                                 \
  if (!std::filesystem::exists(path)) {                    \
    if (!std::filesystem::create_directory(path)) {        \
      Log::Critical("Unable to create directory " + path); \
      return false;                                        \
    }                                                      \
  }

#define touch_helper(path, text)                      \
  if (!std::filesystem::exists(path)) {               \
    std::ofstream os{path};                           \
    os << text;                                       \
    if (os.fail()) {                                  \
      Log::Critical("Unable to create file " + path); \
      return false;                                   \
    }                                                 \
  }

  mkdir_helper(instance_dir_);
  touch_helper(std::string{instance_dir_ + "/token.txt"}, "<BOT TOKEN>");

  return true;
}
