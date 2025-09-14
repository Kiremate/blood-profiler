# Define the agent executable owned by src/

add_executable(agent
		main.cpp
		agent.cpp
		probes/tcp_probe.cpp
		probes/udp_probe.cpp
		probes/https_probe.cpp
		utils/json_reporter.cpp
)

# Local includes
target_include_directories(agent PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link third-party interface libs
target_link_libraries(agent PRIVATE
  OpenSSL::SSL
  OpenSSL::Crypto
  Threads::Threads
  CLI11::CLI11
  nlohmann_json::nlohmann_json
)

# Link Boost (both CONFIG and legacy module cases)
if(TARGET Boost::asio)
	target_link_libraries(agent PRIVATE Boost::asio)
endif()
if(TARGET Boost::system)
	target_link_libraries(agent PRIVATE Boost::system)
endif()

# Ensure headers are visible in legacy FindBoost case
if(DEFINED Boost_INCLUDE_DIRS)
	target_include_directories(agent PRIVATE ${Boost_INCLUDE_DIRS})
endif()

# Windows socket libs
if(WIN32)
	# Target Windows 10 APIs (0x0A00) to avoid _WIN32_WINNT warnings from Asio
	target_compile_definitions(agent PRIVATE _WIN32_WINNT=0x0A00 WINVER=0x0A00)
	target_link_libraries(agent PRIVATE ws2_32 crypt32)
endif()

# Warnings
if(MSVC)
	target_compile_options(agent PRIVATE /W4)
else()
	target_compile_options(agent PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install
install(TARGETS agent DESTINATION bin)