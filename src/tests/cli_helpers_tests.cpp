#include "core/cli_helpers.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

using namespace BloodProfiler::CLIHelpers;
using BloodProfiler::Constants::DEFAULT_HTTPS_HOSTS;
using BloodProfiler::Constants::DEFAULT_TCP_PORTS;
using BloodProfiler::Constants::DEFAULT_UDP_HOSTS;

// Simple minimal test harness
static int failures = 0;
void expect_true(bool cond, const char *msg) {
  if (!cond) {
    std::cerr << "FAIL: " << msg << std::endl;
    ++failures;
  } else {
    std::cerr << "OK: " << msg << std::endl;
  }
}

int main() {
  {
    // validateListArgPairs - missing value after --tcp-ports
    const char *argv[] = {"agent", "--tcp-ports", "--https-hosts",
                          "example.com"};
    expect_true(validateListArgPairs(4, const_cast<char **>(argv)) == false,
                "validateListArgPairs detects missing list after --tcp-ports");
  }
  {
    // validateListArgPairs - proper values
    const char *argv[] = {"agent", "--tcp-ports", "80,443", "--udp-hosts",
                          "8.8.8.8"};
    expect_true(validateListArgPairs(5, const_cast<char **>(argv)) == true,
                "validateListArgPairs accepts proper pairs");
  }
  {
    // addDefaultProbesIfEmpty - injects defaults when all empty
    std::vector<int> tcp;
    std::vector<std::string> https;
    std::vector<std::string> udp;
    addDefaultProbesIfEmpty(tcp, https, udp);
    expect_true(tcp.size() == DEFAULT_TCP_PORTS.size(),
                "default TCP ports injected");
    expect_true(https.size() == DEFAULT_HTTPS_HOSTS.size(),
                "default HTTPS hosts injected");
    expect_true(udp.size() == DEFAULT_UDP_HOSTS.size(),
                "default UDP hosts injected");
  }
  {
    // addDefaultProbesIfEmpty - leaves non-empty untouched
    std::vector<int> tcp{1234};
    std::vector<std::string> https{"example.com"};
    std::vector<std::string> udp; // empty means not all empty -> no injection
    addDefaultProbesIfEmpty(tcp, https, udp);
    expect_true(tcp.size() == 1 && tcp[0] == 1234, "non-empty TCP unchanged");
    expect_true(https.size() == 1 && https[0] == std::string("example.com"),
                "non-empty HTTPS unchanged");
    expect_true(udp.empty(), "UDP unchanged when not all lists are empty");
  }

  if (failures) {
    std::cerr << failures << " failure(s)\n";
    return 1;
  }
  std::cerr << "All tests passed\n";
  return 0;
}
