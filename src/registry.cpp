#include "registry.hpp"

#include <map>
#include <string>
#include <string_view>

#include "log.hpp"

template <typename T>
end::Registry<T>::Registry(std::string_view name) : name_{name} {
  Log::Info("Creating registry " + std::string{name_});
}

template <typename T>
const std::map<std::string, T>& end::Registry<T>::GetValues() const {
  return values_;
}

template <typename T>
const T& end::Registry<T>::GetValue(std::string_view key) const {
  return values_[key];
}

template <typename T>
bool end::Registry<T>::InsertValue(std::string_view key, const T& value) {
  if (!IsValidKey(key)) return false;
  values_[key] = value;
  Log::Info("Inserting " + std::string{key} + " to registry " +
            std::string{name_});
  return true;
}

template <typename T>
bool end::Registry<T>::InsertValue(std::string_view key, const T&& value) {
  if (!IsValidKey(key)) return false;
  values_[key] = value;
  Log::Info("Inserting " + std::string{key} + " to registry " +
            std::string{name_});
  return true;
}

template <typename T>
void end::Registry<T>::Clear() {
  values_.clear();
  Log::Info("Clearing registry " + std::string{name_});
}

template <typename T>
bool end::Registry<T>::IsValidKey(std::string_view key) {
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
