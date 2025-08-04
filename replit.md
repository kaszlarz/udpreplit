# UDP Graph Monitor

## Overview

UDP Graph Monitor is a real-time terminal-based graph visualization tool written in C++ that receives numeric data via UDP and displays it as a live graph using ANSI escape codes. The application provides immediate visual feedback for streaming numeric data, making it useful for monitoring sensors, performance metrics, or any system that outputs numeric values over a network.

## User Preferences

Preferred communication style: Simple, everyday language.
Multi-parameter monitoring: User interested in displaying multiple INA226 sensor values (voltage, current, power) simultaneously.

## System Architecture

### Core Application Design
- **Single-threaded C++ application** using standard library components for maximum portability
- **Terminal-based rendering** using ANSI escape codes for cross-platform compatibility without external graphics libraries
- **Real-time data processing** with continuous UDP listening and immediate graph updates
- **Dynamic terminal size detection** using ioctl(TIOCGWINSZ) with automatic resizing support (minimum 80x20)
- **Configurable time windows** via -m parameter for minutes-based data retention

### Network Communication
- **UDP server architecture** listening on configurable port (default: 4322)
- **POSIX socket implementation** for Linux compatibility
- **Non-blocking or minimal blocking** design to ensure responsive graph updates

### Visualization Engine
- **ASCII/ANSI graph rendering** directly to terminal output
- **Automatic scaling algorithm** to fit data within terminal dimensions
- **Color-coded visualization** using ANSI color codes (green for high values, cyan for low values)
- **Dynamic axis labeling** for numeric value representation

### Build System
- **Make-based build system** with multiple targets (standard, debug, clean, install)
- **Minimal dependency approach** using only C++ standard library
- **C++11 standard compliance** for broad compiler compatibility

### Error Handling and Control
- **Graceful shutdown mechanism** with signal handling for Ctrl+C interruption
- **Terminal resize handling** via SIGWINCH signal for dynamic adaptation
- **Cross-platform design** focused on Linux/POSIX environments

## External Dependencies

### System Requirements
- **C++ compiler** with C++11 support (specifically g++)
- **Make build system** for compilation management
- **Linux/POSIX environment** for socket operations and terminal control
- **ANSI-compatible terminal** for color rendering and escape code support

### Network Dependencies
- **UDP protocol support** through system socket libraries
- **POSIX socket API** for network communication

### No External Libraries
- Project deliberately avoids external dependencies beyond the C++ standard library and system APIs
- No graphics libraries, no third-party networking libraries, no external data visualization frameworks