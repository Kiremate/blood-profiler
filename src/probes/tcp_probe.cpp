#include "tcp_probe.hpp"
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <chrono>

namespace BloodProfiler {

TcpProbe::TcpProbe(const std::string& host, int port, int timeout_ms)
    : host_(host), port_(port), timeout_ms_(timeout_ms) {}

ProbeResult TcpProbe::execute() {
    ProbeResult result;
    result.type = "TCP";
    result.target = host_;
    result.port = port_;
    result.success = false;
    result.response_time_ms = 0;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::socket socket(io_context);
        boost::asio::ip::tcp::resolver resolver(io_context);
        
        // Set timeout
        socket.async_wait(boost::asio::ip::tcp::socket::wait_read, 
            [&](const boost::system::error_code&) {
                socket.close();
            });
        
        // Resolve host
        auto endpoints = resolver.resolve(host_, std::to_string(port_));
        
        // Try to connect
        boost::system::error_code ec;
        boost::asio::connect(socket, endpoints, ec);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.response_time_ms = static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count());
        
        if (!ec) {
            result.success = true;
            socket.close();
        } else {
            result.error_message = ec.message();
        }
        
    } catch (const std::exception& e) {
        auto end_time = std::chrono::high_resolution_clock::now();
        result.response_time_ms = static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count());
        result.error_message = e.what();
    }
    
    return result;
}

} // namespace BloodProfiler