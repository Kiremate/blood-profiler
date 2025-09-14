cmake_minimum_required(VERSION 3.20)
project(blood-profiler VERSION 1.0.0 LANGUAGES CXX)

# Language/compile settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Prefer Boost CONFIG packages (vcpkg). Fall back to legacy FindBoost.
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()
find_package(Boost 1.71 QUIET CONFIG COMPONENTS system)
if(NOT Boost_FOUND)
  message(STATUS "Boost CONFIG not found; falling back to legacy FindBoost (module).")
  cmake_policy(PUSH)
  if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD) # allow module lookup
  endif()
  find_package(Boost REQUIRED COMPONENTS system) # headers in Boost_INCLUDE_DIRS
  cmake_policy(POP)
endif()

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Install: default to a writable location inside the build dir to avoid Program Files
include(GNUInstallDirs)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install prefix" FORCE)
endif()

# Delegate to subdirs
add_subdirectory(third_party)
add_subdirectory(src)