#ifndef ENDIAN_LOG_HPP_
#define ENDIAN_LOG_HPP_

#include <dpp/dispatcher.h>

#include <functional>
#include <string>
#include <string_view>

namespace end {

struct Message {
 public:
  enum Severity {
    trace,
    debug,
    info,
    warn,
    error,
    critical,
  };

  Severity severity;
  std::string message;
};

class Log {
 public:
  static void Print(Message msg);
  static void Print(std::string_view raw_msg);

  static void Trace(std::string_view raw_msg);
  static void Debug(std::string_view raw_msg);
  static void Info(std::string_view raw_msg);
  static void Warn(std::string_view raw_msg);
  static void Error(std::string_view raw_msg);
  static void Critical(std::string_view raw_msg);

  static std::function<void(const dpp::log_t&)> DppLogger();
};

}  // namespace end

#endif  // ENDIAN_LOG_HPP_
