#ifndef ENDIAN_REGISTRY_HPP_
#define ENDIAN_REGISTRY_HPP_

#include <map>
#include <string>

#include "log.hpp"

namespace end {

template <typename T>
class Registry {
 public:
  // name is for debugging and logging purposes only
  explicit Registry(const std::string& name, bool has_namespace = true)
      : name_{name}, has_namespace_{has_namespace} {
    Log::Info("Creating registry " + std::string{name_});
  }

  const std::map<std::string, T>& GetValues() const { return values_; }
  T& GetValue(const std::string& key) { return values_[key]; }

  bool InsertValue(const std::string& key, const T& value) {
    if (!IsValidKey(key, has_namespace_)) return false;
    values_[key] = value;
    Log::Info("Inserting " + key + " to registry " + name_);
    return true;
  }

  void Clear() {
    values_.clear();
    Log::Info("Clearing registry " + std::string{name_});
  }

 private:
  static bool IsValidKey(const std::string& key, bool has_namespace) {
    // true if iterating over the namespace
    bool in_namespace{true};
    for (char c : key) {
      if (c == ':') {
        if (!in_namespace || !has_namespace) return false;
        in_namespace = false;
      } else if (!(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
                   ('0' <= c && c <= '9'))) {
        return false;
      }
    }

    if (in_namespace && has_namespace) return false;
    return true;
  }

  std::string name_{};
  bool has_namespace_{true};
  std::map<std::string, T> values_{};
};

}  // namespace end

#endif  // ENDIAN_REGISTRY_HPP_
