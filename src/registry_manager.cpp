#include "registry_manager.hpp"

#include <dpp/dpp.h>

#include <vector>

#include "bot.hpp"
#include "globals.hpp"
#include "log.hpp"

void end::RegistryManager::RegisterDiscordCommands() {
  dpp::cluster& dbot{end::globals::bot->GetDiscordBot()};
  std::vector<dpp::slashcommand> bulk_cmd_list{};
  bulk_cmd_list.reserve(command_reg.GetValues().size());
  for (const auto& i : command_reg.GetValues()) {
    Log::Info("Registering Discord command: " + i.first);
    bulk_cmd_list.push_back(i.second);
  }
  dbot.global_bulk_command_create(bulk_cmd_list);
}
