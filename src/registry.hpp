#ifndef ENDIAN_REGISTRY_HPP_
#define ENDIAN_REGISTRY_HPP_

#include <map>
#include <string>
#include <string_view>

namespace end {

template <typename T>
class Registry {
 public:
  // name is for debugging and logging purposes only
  explicit Registry(std::string_view name);

  const std::map<std::string, T>& GetValues() const;
  const T& GetValue(std::string_view key) const;

  void InsertValue(std::string_view key, const T& value);
  void InsertValue(std::string_view key, const T&& value);

  void Clear();

 private:
  std::string_view name_{};
  std::map<std::string, T> values_{};
};

}  // namespace end

#endif  // ENDIAN_REGISTRY_HPP_
