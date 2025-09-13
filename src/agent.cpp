#include "agent.hpp"
#include "probes/tcp_probe.hpp"
#include "probes/https_probe.hpp"
#include "probes/udp_probe.hpp"
#include <fstream>
#include <iostream>

namespace BloodProfiler {

Agent::Agent(int timeout_ms) : timeout_ms_(timeout_ms) {}

Agent::~Agent() = default;

void Agent::addTcpProbe(const std::string& host, int port) {
    probes_.push_back(std::make_unique<TcpProbe>(host, port, timeout_ms_));
}

void Agent::addHttpsProbe(const std::string& host, int port, const std::string& sni_override) {
    probes_.push_back(std::make_unique<HttpsProbe>(host, port, timeout_ms_, sni_override));
}

void Agent::addUdpProbe(const std::string& host, int port) {
    probes_.push_back(std::make_unique<UdpProbe>(host, port, timeout_ms_));
}

std::vector<ProbeResult> Agent::runProbes() {
    std::vector<ProbeResult> results;
    results.reserve(probes_.size());
    
    for (auto& probe : probes_) {
        try {
            auto result = probe->execute();
            results.push_back(result);
            std::cout << "Probe " << result.type << " to " << result.target << ":" 
                      << result.port << " - " << (result.success ? "SUCCESS" : "FAILED") 
                      << " (" << result.response_time_ms << "ms)" << std::endl;
        } catch (const std::exception& e) {
            ProbeResult error_result;
            error_result.type = probe->getType();
            error_result.success = false;
            error_result.error_message = e.what();
            error_result.response_time_ms = 0;
            results.push_back(error_result);
        }
    }
    
    return results;
}

void Agent::saveResults(const std::vector<ProbeResult>& results, const std::string& filename) {
    nlohmann::json json_results;
    json_results["timestamp"] = static_cast<int>(std::time(nullptr));
    json_results["total_probes"] = static_cast<int>(results.size());
    
    nlohmann::json probes_array;
    for (const auto& result : results) {
        nlohmann::json probe_json;
        probe_json["type"] = result.type;
        probe_json["target"] = result.target;
        probe_json["port"] = result.port;
        probe_json["success"] = result.success;
        probe_json["response_time_ms"] = result.response_time_ms;
        if (!result.error_message.empty()) {
            probe_json["error"] = result.error_message;
        }
        if (!result.additional_info.empty()) {
            probe_json["additional_info"] = result.additional_info;
        }
        probes_array.push_back(probe_json);
    }
    json_results["probes"] = probes_array;
    
    std::ofstream file(filename);
    if (file.is_open()) {
        file << json_results.dump(2);
        file.close();
    } else {
        std::cerr << "Failed to open output file: " << filename << std::endl;
    }
}

} // namespace BloodProfiler