#include "bot.hpp"

#include <filesystem>
#include <fstream>
#include <string>

#include "log.hpp"

end::Bot::Bot(std::string instance_dir) : end::Bot{instance_dir, true} {}

end::Bot::Bot(std::string instance_dir, bool run_discord)
    : instance_dir_{instance_dir}, run_discord_{run_discord} {
  BuildInstanceDir();
  LoadToken();
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

std::string_view end::Bot::GetToken() { return token_; }

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
