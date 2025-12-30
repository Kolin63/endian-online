#include "mod_loader.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>

#include <dpp/nlohmann/json.hpp>

#include "bot.hpp"
#include "globals.hpp"
#include "log.hpp"
#include "registry_manager.hpp"

void end::ModLoader::LoadMods() {
  const std::string root{end::globals::bot->GetInstanceDir() + "/mods"};
  if (!std::filesystem::exists(root)) return;
  for (const auto& dir_entry : std::filesystem::directory_iterator{root}) {
    LoadMod(dir_entry.path().stem().string());
  }
}

void end::ModLoader::LoadMod(const std::string& name) {
  end::Log::Info("Loading mod " + name);
  const std::string root{end::globals::bot->GetInstanceDir() + "/mods/" + name};
  if (std::filesystem::exists(root + "/data")) LoadData(name);
}

void end::ModLoader::LoadData(const std::string& mod_name) {
  const std::string root{end::globals::bot->GetInstanceDir() + "/mods/" +
                         mod_name + "/data"};
  for (const auto& dir_entry : std::filesystem::directory_iterator{root}) {
    if (dir_entry.is_directory()) {
      LoadDataNamespace(mod_name, dir_entry.path().stem().string());
    }
  }
}

void end::ModLoader::LoadDataNamespace(const std::string& mod_name,
                                       const std::string& namespace_name) {
  const std::string root{end::globals::bot->GetInstanceDir() + "/mods/" +
                         mod_name + "/data/" + namespace_name};

  if (std::filesystem::exists(root + "/commands")) {
    LoadCommands(mod_name, namespace_name);
  }
}

void end::ModLoader::LoadCommands(const std::string& mod_name,
                                  const std::string& namespace_name) {
  const std::string root{end::globals::bot->GetInstanceDir() + "/mods/" +
                         mod_name + "/data/" + namespace_name + "/commands"};
  for (const auto& dir_entry : std::filesystem::directory_iterator{root}) {
    if (dir_entry.path().extension() == ".json") {
      LoadCommand(mod_name, namespace_name, dir_entry.path().stem());
    }
  }
}

void end::ModLoader::LoadCommand(const std::string& mod_name,
                                 const std::string& namespace_name,
                                 const std::string& cmd_name) {
  Log::Info("Loading command " + cmd_name);
  const std::string cmd_path{end::globals::bot->GetInstanceDir() + "/mods/" +
                             mod_name + "/data/" + namespace_name +
                             "/commands/" + cmd_name + ".json"};

  using json = nlohmann::json;

  std::ifstream fs{};
  fs.open(cmd_path);
  json data = json::parse(fs);

  dpp::slashcommand slash{};
  slash = slash.fill_from_json(&data);

  RegistryManager::command_reg.InsertValue(cmd_name, slash);
}
