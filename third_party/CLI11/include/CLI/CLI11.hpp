#pragma once
// Minimal CLI11 implementation for blood-profiler
// In production, use the full CLI11 library from
// https://github.com/CLIUtils/CLI11

#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace CLI {
class ParseError : public std::runtime_error {
public:
  ParseError(const std::string &msg) : std::runtime_error(msg) {}
};

class App {
public:
  App(const std::string &description = "") : desc_(description) {}

  template <typename T>
  App *add_option(const std::string &name, T &variable,
                  const std::string &description = "") {
    // Store references for basic parsing
    if (name == "--tcp-ports") {
      tcp_ports_ref_ = reinterpret_cast<std::vector<int> *>(&variable);
    } else if (name == "--https-hosts") {
      https_hosts_ref_ =
          reinterpret_cast<std::vector<std::string> *>(&variable);
    } else if (name == "--udp-hosts") {
      udp_hosts_ref_ = reinterpret_cast<std::vector<std::string> *>(&variable);
    } else if (name == "--timeout") {
      timeout_ref_ = reinterpret_cast<int *>(&variable);
    } else if (name == "--output") {
      output_ref_ = reinterpret_cast<std::string *>(&variable);
    }
    descriptions_[name] = description;
    return this;
  }

  void parse(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--help" || arg == "-h") {
        printHelp();
        std::exit(0);
      }

      // Basic option parsing
      if (arg.length() > 2 && arg.substr(0, 2) == "--") {
        std::string value;
        if (i + 1 < argc) {
          value = argv[++i];
        }

        if (arg == "--tcp-ports" && tcp_ports_ref_) {
          parseIntVector(*tcp_ports_ref_, value);
        } else if (arg == "--https-hosts" && https_hosts_ref_) {
          parseStringVector(*https_hosts_ref_, value);
        } else if (arg == "--udp-hosts" && udp_hosts_ref_) {
          parseStringVector(*udp_hosts_ref_, value);
        } else if (arg == "--timeout" && timeout_ref_) {
          *timeout_ref_ = std::stoi(value);
        } else if (arg == "--output" && output_ref_) {
          *output_ref_ = value;
        }
      }
    }
  }

  int exit(const ParseError &e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
    return 1;
  }

private:
  std::string desc_;
  std::map<std::string, std::string> descriptions_;

  // Type-specific references
  std::vector<int> *tcp_ports_ref_ = nullptr;
  std::vector<std::string> *https_hosts_ref_ = nullptr;
  std::vector<std::string> *udp_hosts_ref_ = nullptr;
  int *timeout_ref_ = nullptr;
  std::string *output_ref_ = nullptr;

  void printHelp() {
    std::cout << desc_ << std::endl;
    if (!descriptions_.empty()) {
      std::cout << "\nOptions:" << std::endl;
      for (const auto &[name, desc] : descriptions_) {
        std::cout << "  " << name << "\t" << desc << std::endl;
      }
    }
  }

  void parseIntVector(std::vector<int> &variable, const std::string &value) {
    variable.clear();
    std::istringstream iss(value);
    std::string item;
    while (std::getline(iss, item, ',')) {
      variable.push_back(std::stoi(item));
    }
  }

  void parseStringVector(std::vector<std::string> &variable,
                         const std::string &value) {
    variable.clear();
    std::istringstream iss(value);
    std::string item;
    while (std::getline(iss, item, ',')) {
      variable.push_back(item);
    }
  }
};
} // namespace CLI