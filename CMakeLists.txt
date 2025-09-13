cmake_minimum_required(VERSION 3.20)
project(blood-profiler VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find required packages
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)

# Add third-party libraries directory
add_subdirectory(third_party)

# Add source directory
add_subdirectory(src)

# Create the agent executable
add_executable(agent
    src/main.cpp
    src/agent.cpp
    src/probes/tcp_probe.cpp
    src/probes/https_probe.cpp
    src/probes/udp_probe.cpp
    src/utils/json_reporter.cpp
)

target_include_directories(agent PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/CLI11/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/nlohmann_json/include
)

target_link_libraries(agent PRIVATE
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Compiler specific settings
if(MSVC)
    target_compile_options(agent PRIVATE /W4)
else()
    target_compile_options(agent PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install targets
install(TARGETS agent DESTINATION bin)