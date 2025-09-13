#include <iostream>
#include <vector>
#include <string>
#include <CLI/CLI11.hpp>
#include "agent.hpp"

int main(int argc, char** argv) {
    CLI::App app{"Blood Profiler - Cross-platform egress/ingress profiler"};
    
    std::vector<int> tcp_ports;
    std::vector<std::string> https_hosts;
    std::vector<std::string> udp_hosts;
    int timeout = 5000; // 5 seconds default
    std::string output_file = "results.json";
    
    app.add_option("--tcp-ports", tcp_ports, "TCP ports to probe (comma-separated)");
    app.add_option("--https-hosts", https_hosts, "HTTPS hosts to probe (comma-separated)");
    app.add_option("--udp-hosts", udp_hosts, "UDP hosts for echo probe (comma-separated)");
    app.add_option("--timeout", timeout, "Timeout in milliseconds (default: 5000)");
    app.add_option("--output", output_file, "Output JSON file (default: results.json)");
    
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    }
    
    // Create and run the agent
    BloodProfiler::Agent agent(timeout);
    
    // Add default probes if none specified
    if (tcp_ports.empty() && https_hosts.empty() && udp_hosts.empty()) {
        tcp_ports = {22, 80, 443, 8080, 8443};
        https_hosts = {"google.com", "github.com"};
        udp_hosts = {"8.8.8.8", "1.1.1.1"};
    }
    
    // Configure probes
    for (int port : tcp_ports) {
        agent.addTcpProbe("127.0.0.1", port);
    }
    
    for (const std::string& host : https_hosts) {
        agent.addHttpsProbe(host, 443);
    }
    
    for (const std::string& host : udp_hosts) {
        agent.addUdpProbe(host, 53);
    }
    
    // Run all probes
    auto results = agent.runProbes();
    
    // Save results to file
    agent.saveResults(results, output_file);
    
    std::cout << "Probing completed. Results saved to " << output_file << std::endl;
    
    return 0;
}