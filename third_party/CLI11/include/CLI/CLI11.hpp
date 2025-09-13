#pragma once
// Minimal CLI11 implementation for blood-profiler
// In production, use the full CLI11 library from https://github.com/CLIUtils/CLI11

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

namespace CLI {
    class ParseError : public std::runtime_error {
    public:
        ParseError(const std::string& msg) : std::runtime_error(msg) {}
    };
    
    class App {
    public:
        App(const std::string& description = "") : desc_(description) {}
        
        template<typename T>
        App* add_option(const std::string& name, T& variable, const std::string& description = "") {
            // Simplified implementation
            (void)name; (void)variable; (void)description; // Suppress warnings
            return this;
        }
        
        void parse(int argc, char** argv) {
            // Simplified implementation
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "--help" || arg == "-h") {
                    std::cout << desc_ << std::endl;
                    std::exit(0);
                }
            }
        }
        
        int exit(const ParseError& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            return 1;
        }
        
    private:
        std::string desc_;
    };
}