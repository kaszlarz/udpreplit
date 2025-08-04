# UDP Graph Monitor

A real-time terminal-based graph visualization tool that receives numeric data via UDP and displays it using ANSI escape codes.

## Features

- Real-time UDP data reception on configurable port (default: 4322)
- Terminal-based graph rendering with ANSI escape codes
- Automatic scaling and axis labeling
- Color-coded visualization (green for high values, cyan for low values)
- Graceful shutdown with Ctrl+C
- Cross-platform Linux compatibility
- Minimal dependencies (C++ standard library only)

## Building

### Requirements
- C++ compiler with C++11 support (g++)
- Make build system
- Linux environment with POSIX socket support

### Build Commands

```bash
# Build the application
make

# Build with debug symbols
make debug

# Clean build artifacts
make clean

# Install system-wide (optional)
sudo make install
