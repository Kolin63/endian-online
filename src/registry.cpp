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
void end::Registry<T>::InsertValue(std::string_view key, const T& value) {
  values_[key] = value;
  Log::Info("Inserting " + std::string{key} + " to registry " +
            std::string{name_});
}

template <typename T>
void end::Registry<T>::InsertValue(std::string_view key, const T&& value) {
  values_[key] = value;
  Log::Info("Inserting " + std::string{key} + " to registry " +
            std::string{name_});
}

template <typename T>
void end::Registry<T>::Clear() {
  values_.clear();
  Log::Info("Clearing registry " + std::string{name_});
}
