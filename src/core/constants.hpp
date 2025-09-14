// Copyright (c) 2025
// Blood Profiler - Core constants
#pragma once

#include <array>
#include <string>

namespace BloodProfiler::Constants {

// Semantic version of the CLI
inline constexpr const char VERSION[] = "1.0.0";

// Default timeouts and settings
inline constexpr int DEFAULT_TIMEOUT_MS = 5000; // 5 seconds

// Default probe targets used when none are provided
inline constexpr std::array<int, 5> DEFAULT_TCP_PORTS{22, 80, 443, 8080, 8443};
inline constexpr std::array<const char *, 2> DEFAULT_HTTPS_HOSTS{"google.com",
                                                                 "github.com"};
inline constexpr std::array<const char *, 2> DEFAULT_UDP_HOSTS{"8.8.8.8",
                                                               "1.1.1.1"};

} // namespace BloodProfiler::Constants
