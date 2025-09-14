// CLI helper utilities for argument validation and default probe injection
#pragma once

#include "core/constants.hpp"
#include <string>
#include <vector>

namespace BloodProfiler::CLIHelpers {

// Validate that list-taking options are followed by a value
inline bool validateListArgPairs(int argc, char **argv) {
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if ((arg == "--tcp-ports" || arg == "--https-hosts" ||
         arg == "--udp-hosts") &&
        (i + 1 >= argc || argv[i + 1][0] == '-')) {
      return false;
    }
  }
  return true;
}

// Inject default probes when no inputs provided
inline void addDefaultProbesIfEmpty(std::vector<int> &tcp_ports,
                                    std::vector<std::string> &https_hosts,
                                    std::vector<std::string> &udp_hosts) {
  if (tcp_ports.empty() && https_hosts.empty() && udp_hosts.empty()) {
    tcp_ports.assign(Constants::DEFAULT_TCP_PORTS.begin(),
                     Constants::DEFAULT_TCP_PORTS.end());
    https_hosts.assign(Constants::DEFAULT_HTTPS_HOSTS.begin(),
                       Constants::DEFAULT_HTTPS_HOSTS.end());
    udp_hosts.assign(Constants::DEFAULT_UDP_HOSTS.begin(),
                     Constants::DEFAULT_UDP_HOSTS.end());
  }
}

} // namespace BloodProfiler::CLIHelpers
