#pragma once

#include "../agent.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <string>

namespace BloodProfiler {

class HttpsProbe : public ProbeBase {
public:
    HttpsProbe(const std::string& host, int port, int timeout_ms, const std::string& sni_override = "");
    ~HttpsProbe() override = default;
    
    ProbeResult execute() override;
    std::string getType() const override { return "HTTPS"; }
    
private:
    std::string host_;
    int port_;
    int timeout_ms_;
    std::string sni_override_;
};

} // namespace BloodProfiler