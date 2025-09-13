#pragma once

#include "../agent.hpp"
#include <boost/asio.hpp>

namespace BloodProfiler {

class UdpProbe : public ProbeBase {
public:
  UdpProbe(const std::string &host, int port, int timeout_ms);
  ~UdpProbe() override = default;

  ProbeResult execute() override;
  std::string getType() const override { return "UDP"; }

private:
  std::string host_;
  int port_;
  int timeout_ms_;
};

} // namespace BloodProfiler