#include <dpp/dpp.h>

#include <string>

#include "bot.hpp"
#include "log.hpp"

int main(int argc, char** argv) {
  end::Bot endbot{"/home/colin/.local/share/endian/main", true};

  dpp::cluster bot{std::string{endbot.GetToken()}};

  bot.on_log(end::Log::DppLogger());

  bot.on_slashcommand([](const dpp::slashcommand_t& event) {
    if (event.command.get_command_name() == "ping") {
      event.reply("Pong!");
    }
  });

  bot.on_ready([&bot](const dpp::ready_t& event) {
    if (dpp::run_once<struct register_bot_commands>()) {
      bot.global_command_create(
          dpp::slashcommand("ping", "Ping pong!", bot.me.id));
    }
  });

  bot.start(dpp::st_wait);

  return 0;
}
