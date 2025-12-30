#include "mod_loader.hpp"

#include <cassert>
#include <filesystem>
#include <string>

#include "bot.hpp"
#include "globals.hpp"
#include "log.hpp"

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
  end::Log::Debug("Loading namespace " + namespace_name + " from mod " +
                  mod_name);
}

void end::ModLoader::LoadCommands() {}
