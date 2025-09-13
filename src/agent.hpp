#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace BloodProfiler {

struct ProbeResult {
  std::string type;
  std::string target;
  int port;
  bool success;
  int response_time_ms;
  std::string error_message;
  std::string additional_info;
};

class ProbeBase {
public:
  virtual ~ProbeBase() = default;
  virtual ProbeResult execute() = 0;
  virtual std::string getType() const = 0;
};

class Agent {
public:
  explicit Agent(int timeout_ms = 5000);
  ~Agent();

  void addTcpProbe(const std::string &host, int port);
  void addHttpsProbe(const std::string &host, int port,
                     const std::string &sni_override = "");
  void addUdpProbe(const std::string &host, int port);

  std::vector<ProbeResult> runProbes();
  void saveResults(const std::vector<ProbeResult> &results,
                   const std::string &filename);

private:
  int timeout_ms_;
  std::vector<std::unique_ptr<ProbeBase>> probes_;
};

} // namespace BloodProfiler