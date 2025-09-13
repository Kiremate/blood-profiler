#include "udp_probe.hpp"
#include <array>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <chrono>
#include <thread>

namespace BloodProfiler {

UdpProbe::UdpProbe(const std::string &host, int port, int timeout_ms)
    : host_(host), port_(port), timeout_ms_(timeout_ms) {}

ProbeResult UdpProbe::execute() {
  ProbeResult result;
  result.type = "UDP";
  result.target = host_;
  result.port = port_;
  result.success = false;
  result.response_time_ms = 0;

  auto start_time = std::chrono::high_resolution_clock::now();

  try {
    boost::asio::io_context io_context;
    boost::asio::ip::udp::socket socket(io_context);
    boost::asio::ip::udp::resolver resolver(io_context);

    socket.open(boost::asio::ip::udp::v4());

    // Resolve endpoint
    auto endpoints = resolver.resolve(boost::asio::ip::udp::v4(), host_,
                                      std::to_string(port_));
    auto endpoint = *endpoints.begin();

    // Create a simple DNS query for UDP port 53 echo test
    std::array<char, 12> dns_query = {
        0x12, 0x34, // Transaction ID
        0x01, 0x00, // Flags (standard query)
        0x00, 0x01, // Questions
        0x00, 0x00, // Answer RRs
        0x00, 0x00, // Authority RRs
        0x00, 0x00  // Additional RRs
    };

    // Send query
    socket.send_to(boost::asio::buffer(dns_query), endpoint);

    // Try to receive response with timeout
    std::array<char, 512> response_buffer;
    boost::asio::ip::udp::endpoint sender_endpoint;

    // Set socket to non-blocking and use timeout
    socket.non_blocking(true);

    boost::system::error_code ec;
    size_t bytes_received = 0;

    auto timeout_time = start_time + std::chrono::milliseconds(timeout_ms_);

    while (std::chrono::high_resolution_clock::now() < timeout_time) {
      bytes_received = socket.receive_from(boost::asio::buffer(response_buffer),
                                           sender_endpoint, 0, ec);

      if (!ec && bytes_received > 0) {
        result.success = true;
        break;
      }

      if (ec == boost::asio::error::would_block) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        continue;
      }

      if (ec) {
        result.error_message = ec.message();
        break;
      }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    result.response_time_ms =
        static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
                             end_time - start_time)
                             .count());

    if (result.success) {
      result.additional_info =
          "Received " + std::to_string(bytes_received) + " bytes";
    }

  } catch (const std::exception &e) {
    auto end_time = std::chrono::high_resolution_clock::now();
    result.response_time_ms =
        static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(
                             end_time - start_time)
                             .count());
    result.error_message = e.what();
  }

  return result;
}

} // namespace BloodProfiler