# blood-profiler

Cross-platform egress/ingress profiler (TCP/UDP/HTTPS/DNS/ICMP) with OPSEC guardrails. For authorized testing only.

## Overview

Blood Profiler is a C++17 cross-platform network connectivity testing tool designed for security professionals. It provides MVP probes for:

- **TCP Connect**: Test connectivity to configurable ports
- **HTTPS GET**: HTTP over SSL with SNI/Host override support
- **UDP Echo**: DNS queries to port 53 for UDP connectivity testing

All probes include configurable timeouts and generate detailed JSON reports with timing information, success/failure status, and error details.

## Features

✅ **Cross-platform**: Builds on Ubuntu, Windows, and other platforms
✅ **Modern C++17**: Clean, maintainable codebase
✅ **Async I/O**: Uses Boost.Asio for efficient network operations
✅ **SSL/TLS Support**: OpenSSL integration for HTTPS probes
✅ **JSON Output**: Structured reporting with timestamps and metrics
✅ **CLI Interface**: Easy-to-use command-line interface
✅ **CI/CD Ready**: GitHub Actions for automated builds

## Quick Start

### Prerequisites

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake ninja-build libboost-system-dev libssl-dev pkg-config
```

#### Windows
- Visual Studio 2022 or newer
- vcpkg for dependencies:
  ```cmd
  vcpkg install boost-system:x64-windows openssl:x64-windows
  ```

### Building

```bash
# Configure
cmake --preset linux-default    # Linux
cmake --preset windows-default  # Windows

# Build
cmake --build --preset linux-default    # Linux
cmake --build --preset windows-default  # Windows
```

### Usage

#### Basic Usage (Default Probes)
```bash
./agent
```
Runs default probes:
- TCP: ports 22, 80, 443, 8080, 8443 on localhost
- HTTPS: google.com, github.com
- UDP: DNS queries to 8.8.8.8, 1.1.1.1

#### Custom Probes
```bash
# TCP port scanning
./agent --tcp-ports 22,80,443,993,995

# HTTPS testing with custom hosts
./agent --https-hosts example.com,secure.example.org

# UDP connectivity testing
./agent --udp-hosts 8.8.8.8,1.1.1.1,208.67.222.222

# Custom timeout (5 seconds default)
./agent --timeout 10000 --output custom_results.json
```

#### Help
```bash
./agent --help
```

## Output Format

Results are saved as JSON with detailed timing and error information:

```json
{
  "timestamp": 1757772904,
  "total_probes": 9,
  "probes": [
    {
      "type": "TCP",
      "target": "127.0.0.1",
      "port": 22,
      "success": true,
      "response_time_ms": 2
    },
    {
      "type": "HTTPS",
      "target": "github.com",
      "port": 443,
      "success": true,
      "response_time_ms": 166,
      "additional_info": "HTTP 200 OK"
    },
    {
      "type": "UDP",
      "target": "8.8.8.8",
      "port": 53,
      "success": false,
      "response_time_ms": 5000,
      "error": "Operation timed out"
    }
  ]
}
```

## Architecture

```
blood-profiler/
├── src/
│   ├── main.cpp              # CLI entry point
│   ├── agent.{hpp,cpp}       # Main agent orchestrator
│   ├── probes/               # Probe implementations
│   │   ├── tcp_probe.{hpp,cpp}
│   │   ├── https_probe.{hpp,cpp}
│   │   └── udp_probe.{hpp,cpp}
│   └── utils/                # Utilities
├── third_party/              # Vendored dependencies
│   ├── CLI11/                # Command line parsing
│   └── nlohmann_json/        # JSON serialization
├── CMakeLists.txt            # Build configuration
├── CMakePresets.json         # Build presets
└── .github/workflows/        # CI/CD
```

## Dependencies

- **Boost.Asio**: Async I/O and networking  
- **OpenSSL**: SSL/TLS support for HTTPS probes
- **CLI11**: Command-line argument parsing (vendored)
- **nlohmann/json**: JSON serialization (vendored)

## Development

### Building for Development
```bash
# Debug build
cmake --preset debug
cmake --build --preset debug
```

### Code Formatting (clang-format)

To ensure consistent code style, use **clang-format** for C++ files. This project recommends the [Clang-Format](https://marketplace.visualstudio.com/items?itemName=xaver.clang-format) VS Code extension.

#### Install clang-format

- **Linux (Ubuntu/Debian):**
  ```bash
  sudo apt-get install clang-format
  ```
- **Windows:**
  Install LLVM (includes clang-format) using [winget](https://learn.microsoft.com/en-us/windows/package-manager/winget/):
  ```powershell
  winget install -e --id LLVM.LLVM
  ```
  The executable will be at `C:\Program Files\LLVM\bin\clang-format.exe`.

#### VS Code Setup

1. Install the **Clang-Format** extension (`xaver.clang-format`).
2. Set as your default formatter for C++ files.
3. (Optional) Add to your `settings.json`:
   ```json
   {
     "editor.formatOnSave": true,
     "clang-format.executable": "C:\\Program Files\\LLVM\\bin\\clang-format.exe"
   }
   ```
   Adjust the path for your OS if needed.

#### Usage

- Format a file: Right-click and select **Format Document** or use `Shift+Alt+F`.
- Format on save: Enable `editor.formatOnSave` in settings.

### Adding New Probes
1. Create new probe class inheriting from `ProbeBase`
2. Implement `execute()` and `getType()` methods
3. Add probe creation method to `Agent` class
4. Update CLI argument parsing in `main.cpp`

### Testing
```bash
# Basic functionality test
./agent --help
./agent --tcp-ports 80 --timeout 1000
```

## Security Considerations

⚠️ **IMPORTANT**: This tool is designed for authorized security testing only. Users are responsible for:

- Obtaining proper authorization before scanning
- Complying with applicable laws and regulations
- Respecting rate limits and being considerate of target systems
- Not using this tool for malicious purposes

## License

Apache License 2.0 - See [LICENSE](LICENSE) for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Ensure tests pass
5. Submit a pull request

## CI/CD

Automated builds run on:
- ✅ Ubuntu 20.04+ (GCC)
- ✅ Windows Server 2022 (MSVC)

Build artifacts are available in GitHub Actions.
