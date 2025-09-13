#include "https_probe.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/system/error_code.hpp>
#include <chrono>
#include <sstream>

namespace BloodProfiler {

HttpsProbe::HttpsProbe(const std::string& host, int port, int timeout_ms, const std::string& sni_override)
    : host_(host), port_(port), timeout_ms_(timeout_ms), sni_override_(sni_override) {}

ProbeResult HttpsProbe::execute() {
    ProbeResult result;
    result.type = "HTTPS";
    result.target = host_;
    result.port = port_;
    result.success = false;
    result.response_time_ms = 0;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        boost::asio::io_context io_context;
        boost::asio::ssl::context ssl_context(boost::asio::ssl::context::sslv23);
        ssl_context.set_default_verify_paths();
        
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket(io_context, ssl_context);
        boost::asio::ip::tcp::resolver resolver(io_context);
        
        // Set SNI
        std::string sni_host = sni_override_.empty() ? host_ : sni_override_;
        SSL_set_tlsext_host_name(ssl_socket.native_handle(), sni_host.c_str());
        
        // Resolve and connect
        auto endpoints = resolver.resolve(host_, std::to_string(port_));
        boost::asio::connect(ssl_socket.lowest_layer(), endpoints);
        
        // SSL handshake
        ssl_socket.handshake(boost::asio::ssl::stream_base::client);
        
        // Send HTTP GET request
        std::ostringstream request_stream;
        request_stream << "GET / HTTP/1.1\r\n";
        request_stream << "Host: " << (sni_override_.empty() ? host_ : sni_override_) << "\r\n";
        request_stream << "Connection: close\r\n\r\n";
        
        std::string request = request_stream.str();
        boost::asio::write(ssl_socket, boost::asio::buffer(request));
        
        // Read response
        boost::asio::streambuf response;
        boost::system::error_code ec;
        size_t bytes_read = boost::asio::read_until(ssl_socket, response, "\r\n", ec);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.response_time_ms = static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count());
        
        if (bytes_read > 0) {
            result.success = true;
            std::istream response_stream(&response);
            std::string http_version;
            unsigned int status_code;
            std::string status_message;
            
            response_stream >> http_version >> status_code >> status_message;
            result.additional_info = "HTTP " + std::to_string(status_code) + " " + status_message;
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