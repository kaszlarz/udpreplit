#include <iostream>
#include <string>
#include <cstring>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "udp_listener.h"
#include "terminal_graph.h"
#include "data_parser.h"

// Global variables for signal handling
bool running = true;
bool terminal_resized = false;
UDPListener* listener = nullptr;
TerminalGraph* graph = nullptr;

void signalHandler(int signum) {
    if (signum == SIGWINCH) {
        // Terminal was resized
        terminal_resized = true;
    } else {
        // SIGINT or SIGTERM
        std::cout << "\nShutting down gracefully..." << std::endl;
        running = false;
        if (listener) {
            listener->stop();
        }
    }
}

// Function to get terminal size using ioctl
void getTerminalSize(int& width, int& height) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        width = w.ws_col;
        height = w.ws_row;
        
        // Ensure minimum size
        if (width < 80) width = 80;
        if (height < 20) height = 20;
    } else {
        // Fallback to default if ioctl fails
        width = 80;
        height = 20;
    }
}

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS]\n"
              << "Options:\n"
              << "  -p PORT    UDP port to listen on (default: 4322)\n"
              << "  -m MINUTES Graph width in minutes of data (default: auto-detect)\n"
              << "  -h         Show this help message\n"
              << "\nGraph Display:\n"
              << "  Terminal size is auto-detected (minimum 80x20)\n"
              << "  Graph width can be specified in minutes for time-based data\n"
              << "  Example: -m 60 shows last 60 minutes of data points\n"
              << "\nData Format:\n"
              << "  Send numeric values as plain text over UDP\n"
              << "  Multiple values can be sent separated by newlines or spaces\n"
              << "  Example: echo \"42.5\" | nc -u localhost 4322\n";
}

int main(int argc, char* argv[]) {
    int port = 4322;
    int minutes = 0; // 0 means auto-detect based on terminal width
    
    // Parse command line arguments
    int opt;
    while ((opt = getopt(argc, argv, "p:m:h")) != -1) {
        switch (opt) {
            case 'p':
                port = std::atoi(optarg);
                if (port <= 0 || port > 65535) {
                    std::cerr << "Error: Invalid port number. Must be between 1 and 65535." << std::endl;
                    return 1;
                }
                break;
            case 'm':
                minutes = std::atoi(optarg);
                if (minutes <= 0) {
                    std::cerr << "Error: Minutes must be a positive number." << std::endl;
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
    
    // Set up signal handlers for graceful shutdown and terminal resize
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGWINCH, signalHandler);
    
    try {
        // Get terminal size
        int term_width, term_height;
        getTerminalSize(term_width, term_height);
        
        // Initialize components
        listener = new UDPListener(port);
        graph = new TerminalGraph(term_width, term_height, minutes);
        DataParser parser;
        
        std::cout << "UDP Graph Monitor starting on port " << port << std::endl;
        if (minutes > 0) {
            std::cout << "Time window: " << minutes << " minutes" << std::endl;
        }
        std::cout << "Terminal size: " << term_width << "x" << term_height << std::endl;
        std::cout << "Press Ctrl+C to exit\n" << std::endl;
        
        // Clear screen and hide cursor
        std::cout << "\033[2J\033[H\033[?25l";
        std::cout.flush();
        
        // Main event loop
        while (running) {
            // Check if terminal was resized
            if (terminal_resized) {
                int new_width, new_height;
                getTerminalSize(new_width, new_height);
                graph->updateTerminalSize(new_width, new_height);
                terminal_resized = false;
                
                // Force a redraw
                std::cout << "\033[2J\033[H"; // Clear screen and move to top
                if (graph->getDataPointCount() > 0) {
                    graph->render();
                    std::cout.flush();
                }
            }
            
            std::string data = listener->receiveData(1000); // 1 second timeout
            
            if (!data.empty()) {
                std::vector<double> values = parser.parseData(data);
                
                for (double value : values) {
                    graph->addDataPoint(value);
                }
                
                if (!values.empty()) {
                    // Move cursor to top and redraw graph
                    std::cout << "\033[H";
                    graph->render();
                    std::cout.flush();
                }
            }
        }
        
        // Restore cursor and clean up
        std::cout << "\033[?25h" << std::endl;
        delete listener;
        delete graph;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        if (listener) {
            delete listener;
        }
        if (graph) {
            delete graph;
        }
        // Restore cursor
        std::cout << "\033[?25h" << std::endl;
        return 1;
    }
    
    return 0;
}
