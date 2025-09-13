#pragma once

#include "../agent.hpp"
#include <boost/asio.hpp>
#include <chrono>

namespace BloodProfiler {

class TcpProbe : public ProbeBase {
public:
    TcpProbe(const std::string& host, int port, int timeout_ms);
    ~TcpProbe() override = default;
    
    ProbeResult execute() override;
    std::string getType() const override { return "TCP"; }
    
private:
    std::string host_;
    int port_;
    int timeout_ms_;
};

} // namespace BloodProfiler