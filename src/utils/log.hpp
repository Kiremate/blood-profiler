// Simple logging helpers (stderr-based)
#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string_view>

namespace BloodProfiler::Log {

enum class Level { Info, Warn, Error, Debug };

inline const char *to_string(Level lvl) {
  switch (lvl) {
  case Level::Info:
    return "INFO";
  case Level::Warn:
    return "WARN";
  case Level::Error:
    return "ERROR";
  case Level::Debug:
    return "DEBUG";
  }
  return "?";
}

inline std::string timestamp() {
  using clock = std::chrono::system_clock;
  const auto now = clock::now();
  const std::time_t t = clock::to_time_t(now);
  std::tm tm_buf{};
#if defined(_WIN32)
  localtime_s(&tm_buf, &t);
#else
  localtime_r(&t, &tm_buf);
#endif
  std::ostringstream oss;
  oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

inline void log(Level lvl, std::string_view msg) {
  std::cerr << '[' << timestamp() << "] " << to_string(lvl) << ": " << msg
            << std::endl;
}

inline void info(std::string_view msg) { log(Level::Info, msg); }
inline void warn(std::string_view msg) { log(Level::Warn, msg); }
inline void error(std::string_view msg) { log(Level::Error, msg); }
inline void debug(std::string_view msg) { log(Level::Debug, msg); }

} // namespace BloodProfiler::Log
