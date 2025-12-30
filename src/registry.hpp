#ifndef ENDIAN_REGISTRY_HPP_
#define ENDIAN_REGISTRY_HPP_

#include <map>
#include <string>
#include <string_view>

#include "log.hpp"

namespace end {

template <typename T>
class Registry {
 public:
  // name is for debugging and logging purposes only
  explicit Registry(std::string_view name) : name_{name} {
    Log::Info("Creating registry " + std::string{name_});
  }

  const std::map<std::string, T>& GetValues() const { return values_; }
  const T& GetValue(std::string_view key) const { return values_[key]; }

  bool InsertValue(std::string_view key, const T& value) {
    if (!IsValidKey(key)) return false;
    values_[key] = value;
    Log::Info("Inserting " + std::string{key} + " to registry " +
              std::string{name_});
    return true;
  }

  bool InsertValue(std::string_view key, const T&& value) {
    if (!IsValidKey(key)) return false;
    values_[key] = value;
    Log::Info("Inserting " + std::string{key} + " to registry " +
              std::string{name_});
    return true;
  }

  void Clear() {
    values_.clear();
    Log::Info("Clearing registry " + std::string{name_});
  }

 private:
  static bool IsValidKey(std::string_view key) {
    // true if iterating over the namespace
    bool in_namespace{true};
    for (char c : key) {
      if (c == ':') {
        if (!in_namespace) return false;
        in_namespace = false;
      } else if (!(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
                   ('0' <= c && c <= '9'))) {
        return false;
      }
    }

    if (in_namespace) return false;
    return true;
  }

  std::string_view name_{};
  std::map<std::string, T> values_{};
};

}  // namespace end

#endif  // ENDIAN_REGISTRY_HPP_
