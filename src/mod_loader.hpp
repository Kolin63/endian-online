#ifndef ENDIAN_MOD_LOADER_HPP_
#define ENDIAN_MOD_LOADER_HPP_

#include <string>

namespace end {

class ModLoader {
 public:
  static void LoadMods();

 private:
  static void LoadMod(const std::string& name);
  static void LoadData(const std::string& mod_name);
  static void LoadDataNamespace(const std::string& mod_name,
                                const std::string& namespace_name);
  static void LoadCommands(const std::string& mod_name,
                           const std::string& namespace_name);
  static void LoadCommand(const std::string& mod_name,
                          const std::string& namespace_name,
                          const std::string& cmd_name);
};

}  // namespace end

#endif  // ENDIAN_MOD_LOADER_HPP_
