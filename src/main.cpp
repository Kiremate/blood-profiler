#include "agent.hpp"
#include "core/cli_helpers.hpp"
#include "core/constants.hpp"
#include "utils/log.hpp"
#include <CLI/CLI.hpp>
#include <cassert>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>


namespace {

using BloodProfiler::Log::debug;
using BloodProfiler::Log::error;
using BloodProfiler::Log::info;
using BloodProfiler::Log::warn;

void printConsentAndExit() {
  std::cout << "Blood Profiler v" << BloodProfiler::Constants::VERSION << "\n"
            << "This tool performs network probes to assess egress and "
               "ingress capabilities.\n"
            << "By using this tool, you consent to sending network traffic to "
               "the specified targets.\n"
            << "If you do not consent, please exit now.\n";
}

bool validateListArgPairs(int argc, char **argv) {
  if (!BloodProfiler::CLIHelpers::validateListArgPairs(argc, argv)) {
    std::cerr << "Error: --tcp-ports/--https-hosts/--udp-hosts require an "
                 "argument (comma-separated list)\n";
    return false;
  }
  return true;
}

// Use helper from header
using BloodProfiler::CLIHelpers::addDefaultProbesIfEmpty;

void configureProbes(BloodProfiler::Agent &agent,
                     const std::vector<int> &tcp_ports,
                     const std::vector<std::string> &https_hosts,
                     const std::vector<std::string> &udp_hosts) {
  for (int port : tcp_ports) {
    assert(port > 0 && port <= 65535);
    agent.addTcpProbe("127.0.0.1", port);
  }
  for (const std::string &host : https_hosts) {
    assert(!host.empty());
    agent.addHttpsProbe(host, 443);
  }
  for (const std::string &host : udp_hosts) {
    assert(!host.empty());
    agent.addUdpProbe(host, 53);
  }
}

} // namespace

int main(int argc, char **argv) {
  CLI::App app{"Blood Profiler - Cross-platform egress/ingress profiler"};

  std::vector<int> tcp_ports;
  std::vector<std::string> https_hosts;
  std::vector<std::string> udp_hosts;
  int timeout = BloodProfiler::Constants::DEFAULT_TIMEOUT_MS; // default
  std::string output_file;         // Only used when --output is provided
  bool compact_deprecated = false; // Deprecated flag for compatibility
  std::string format = "json";     // json or ndjson

  // If user passes nothing we print consent message and exit non-zer0
  if (argc == 1) {
    printConsentAndExit();
    return 1;
  }
  // If user asks for TCP/UDP without destination (controller) clear error and
  // exit
  if (argc > 1) {
    if (!validateListArgPairs(argc, argv)) {
      return 1;
    }
  }
  // HTTPS: verify certs by default; --insecure is explicit opt-in; allow
  // --ca-file/--ca-path.
  app.add_option("--tcp-ports", tcp_ports,
                 "TCP ports to probe (comma-separated)")
      ->delimiter(',');
  app.add_option("--https-hosts", https_hosts,
                 "HTTPS hosts to probe (comma-separated)")
      ->delimiter(',');
  app.add_option("--udp-hosts", udp_hosts,
                 "UDP hosts for echo probe (comma-separated)")
      ->delimiter(',');
  app.add_option("--timeout", timeout,
                 "Timeout in milliseconds (default: 5000)");
  app.add_option("--format", format,
                 "Output format: json|ndjson (default: json)")
      ->check(CLI::IsMember({"json", "ndjson"}, CLI::ignore_case));
  CLI::Option *out_opt =
      app.add_option("--output", output_file,
                     "Also write JSON to the specified file path (pretty). Use "
                     "'-' to target stdout (redundant).");
  // Backward compatibility: accept --compact but ignore it; we always pretty
  // print now
  app.add_flag(
      "--compact", compact_deprecated,
      "DEPRECATED: Output is always pretty-printed; this flag is ignored");

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  // Create and run the agent
  BloodProfiler::Agent agent(timeout);

  // Add default probes if none specified
  addDefaultProbesIfEmpty(tcp_ports, https_hosts, udp_hosts);

  // Configure probes
  try {
    configureProbes(agent, tcp_ports, https_hosts, udp_hosts);
  } catch (const std::exception &ex) {
    error(std::string("Failed to configure probes: ") + ex.what());
    return 2;
  } catch (...) {
    error("Failed to configure probes: unknown error");
    return 2;
  }

  // Run all probes
  std::vector<BloodProfiler::ProbeResult> results;
  try {
    results = agent.runProbes();
  } catch (const std::exception &ex) {
    error(std::string("Failed while running probes: ") + ex.what());
    return 3;
  } catch (...) {
    error("Failed while running probes: unknown error");
    return 3;
  }

  // Emit results based on format
  if (format == "json" || format == "JSON" || format == "Json") {
    try {
      agent.saveResults(results, "-", /*pretty_print=*/true);
    } catch (const std::exception &ex) {
      error(std::string("Failed to serialize results to stdout: ") + ex.what());
      return 4;
    }
  } else {
    // ndjson
    try {
      // Write to stdout
      for (const auto &r : results) {
        nlohmann::json j;
        j["type"] = r.type;
        j["target"] = r.target;
        j["port"] = r.port;
        j["success"] = r.success;
        j["response_time_ms"] = r.response_time_ms;
        if (!r.error_message.empty())
          j["error"] = r.error_message;
        if (!r.additional_info.empty())
          j["additional_info"] = r.additional_info;
        std::cout << j.dump(-1, ' ', false,
                            nlohmann::json::error_handler_t::replace)
                  << "\n";
      }
    } catch (const std::exception &ex) {
      error(std::string("Failed to serialize NDJSON to stdout: ") + ex.what());
      return 4;
    }
  }

  // If user provided --output and it's not "-", also write that file (pretty)
  if (out_opt->count() > 0) {
    if (output_file != "-") {
      try {
        if (format == "json" || format == "JSON" || format == "Json") {
          agent.saveResults(results, output_file, /*pretty_print=*/true);
        } else {
          std::ofstream ofs(output_file);
          if (!ofs.is_open()) {
            throw std::runtime_error("cannot open output file");
          }
          for (const auto &r : results) {
            nlohmann::json j;
            j["type"] = r.type;
            j["target"] = r.target;
            j["port"] = r.port;
            j["success"] = r.success;
            j["response_time_ms"] = r.response_time_ms;
            if (!r.error_message.empty())
              j["error"] = r.error_message;
            if (!r.additional_info.empty())
              j["additional_info"] = r.additional_info;
            ofs << j.dump(-1, ' ', false,
                          nlohmann::json::error_handler_t::replace)
                << '\n';
          }
        }
        info(std::string("Probing completed. Printed ") +
             (format == "ndjson" ? "NDJSON" : "pretty JSON") +
             " to stdout and saved to " + output_file);
      } catch (const std::exception &ex) {
        error(std::string("Failed to save results to file '") + output_file +
              "': " + ex.what());
        return 5;
      }
    } else {
      info(std::string("Probing completed. Printed ") +
           (format == "ndjson" ? "NDJSON" : "pretty JSON") + " to stdout");
    }
  } else {
    info(std::string("Probing completed. Printed ") +
         (format == "ndjson" ? "NDJSON" : "pretty JSON") + " to stdout");
  }
  return 0;
}