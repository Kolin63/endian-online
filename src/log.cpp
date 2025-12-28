#include "log.hpp"

#include <dpp/misc-enum.h>

#include <ctime>
#include <iostream>
#include <string>
#include <string_view>

constexpr std::string_view GetSeverityString(end::Message::Severity type) {
  switch (type) {
    case end::Message::trace:
      return "TRACE";
    case end::Message::debug:
      return "DEBUG";
    case end::Message::info:
      return "INFO";
    case end::Message::warn:
      return "WARN";
    case end::Message::error:
      return "ERROR";
    case end::Message::critical:
      return "CRITICAL";
    default:
      return "???";
  }
}

void end::Log::Print(Message msg) {
  // time
  // [2025-12-27 20:57:00]
  char time_buf[20]{};
  std::time_t time{std::time(nullptr)};
  std::strftime(time_buf, sizeof(time_buf), "%F %T", std::localtime(&time));

  std::cout << '[' << time_buf << "] [" << GetSeverityString(msg.severity)
            << "] " << msg.message << '\n';
}

void end::Log::Print(std::string_view raw_msg) {
  Print(Message{Message::info, std::string{raw_msg}});
}

void end::Log::Trace(std::string_view raw_msg) {
  Print(Message{Message::trace, std::string{raw_msg}});
}

void end::Log::Debug(std::string_view raw_msg) {
  Print(Message{Message::debug, std::string{raw_msg}});
}

void end::Log::Info(std::string_view raw_msg) {
  Print(Message{Message::info, std::string{raw_msg}});
}

void end::Log::Warn(std::string_view raw_msg) {
  Print(Message{Message::warn, std::string{raw_msg}});
}

void end::Log::Error(std::string_view raw_msg) {
  Print(Message{Message::error, std::string{raw_msg}});
}

void end::Log::Critical(std::string_view raw_msg) {
  Print(Message{Message::critical, std::string{raw_msg}});
}

std::function<void(const dpp::log_t&)> end::Log::DppLogger() {
  return [](const dpp::log_t& event) {
    end::Message::Severity sev{};
    switch (event.severity) {
      case dpp::ll_trace:
        sev = end::Message::trace;
        break;
      case dpp::ll_debug:
        sev = end::Message::debug;
        break;
      case dpp::ll_info:
        sev = end::Message::info;
        break;
      case dpp::ll_warning:
        sev = end::Message::warn;
        break;
      case dpp::ll_error:
        sev = end::Message::error;
        break;
      case dpp::ll_critical:
        sev = end::Message::critical;
        break;
    }
    end::Log::Print({sev, event.message});
  };
}
