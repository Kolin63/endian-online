#ifndef ENDIAN_REGISTRY_MANAGER_HPP_
#define ENDIAN_REGISTRY_MANAGER_HPP_

#include <dpp/appcommand.h>

#include "registry.hpp"

namespace end {

class RegistryManager {
 public:
  static inline Registry<dpp::slashcommand> command_reg{"command_reg", false};

  static void RegisterDiscordCommands();
};

}  // namespace end

#endif  // ENDIAN_REGISTRY_MANAGER_HPP_
