#include <iostream>
#include <string>
#include <cstring>
#include <signal.h>
#include <unistd.h>
#include "udp_listener.h"
#include "terminal_graph.h"
#include "data_parser.h"

// Global variables for signal handling
bool running = true;
UDPListener* listener = nullptr;

void signalHandler(int signum) {
    std::cout << "\nShutting down gracefully..." << std::endl;
    running = false;
    if (listener) {
        listener->stop();
    }
}

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS]\n"
              << "Options:\n"
              << "  -p PORT    UDP port to listen on (default: 4322)\n"
              << "  -h         Show this help message\n"
              << "\nData Format:\n"
              << "  Send numeric values as plain text over UDP\n"
              << "  Multiple values can be sent separated by newlines or spaces\n"
              << "  Example: echo \"42.5\" | nc -u localhost 4322\n";
}

int main(int argc, char* argv[]) {
    int port = 4322;
    
    // Parse command line arguments
    int opt;
    while ((opt = getopt(argc, argv, "p:h")) != -1) {
        switch (opt) {
            case 'p':
                port = std::atoi(optarg);
                if (port <= 0 || port > 65535) {
                    std::cerr << "Error: Invalid port number. Must be between 1 and 65535." << std::endl;
                    return 1;
                }
                break;
            case 'h':
                printUsage(argv[0]);
                return 0;
            case '?':
                std::cerr << "Error: Unknown option or missing argument." << std::endl;
                printUsage(argv[0]);
                return 1;
        }
    }
    
    // Set up signal handlers for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    try {
        // Initialize components
        listener = new UDPListener(port);
        TerminalGraph graph(80, 20); // 80 columns, 20 rows
        DataParser parser;
        
        std::cout << "UDP Graph Monitor starting on port " << port << std::endl;
        std::cout << "Press Ctrl+C to exit\n" << std::endl;
        
        // Clear screen and hide cursor
        std::cout << "\033[2J\033[H\033[?25l";
        std::cout.flush();
        
        // Main event loop
        while (running) {
            std::string data = listener->receiveData(1000); // 1 second timeout
            
            if (!data.empty()) {
                std::vector<double> values = parser.parseData(data);
                
                for (double value : values) {
                    graph.addDataPoint(value);
                }
                
                if (!values.empty()) {
                    // Move cursor to top and redraw graph
                    std::cout << "\033[H";
                    graph.render();
                    std::cout.flush();
                }
            }
        }
        
        // Restore cursor and clean up
        std::cout << "\033[?25h" << std::endl;
        delete listener;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        if (listener) {
            delete listener;
        }
        // Restore cursor
        std::cout << "\033[?25h" << std::endl;
        return 1;
    }
    
    return 0;
}
