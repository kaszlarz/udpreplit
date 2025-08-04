#include "udp_listener.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/select.h>
#include <stdexcept>
#include <errno.h>

UDPListener::UDPListener(int port) : is_running(false) {
    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
    }
    
    // Set socket options for reuse
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(sockfd);
        throw std::runtime_error("Failed to set socket options: " + std::string(strerror(errno)));
    }
    
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    // Bind socket to address
    if (bind(sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sockfd);
        throw std::runtime_error("Failed to bind socket to port " + std::to_string(port) + ": " + std::string(strerror(errno)));
    }
    
    is_running = true;
}

UDPListener::~UDPListener() {
    if (sockfd >= 0) {
        close(sockfd);
    }
}

std::string UDPListener::receiveData(int timeout_ms) {
    if (!is_running) {
        return "";
    }
    
    fd_set readfds;
    struct timeval timeout;
    
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    
    // Set timeout
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;
    
    int activity = select(sockfd + 1, &readfds, nullptr, nullptr, timeout_ms > 0 ? &timeout : nullptr);
    
    if (activity < 0) {
        if (errno != EINTR) {
            throw std::runtime_error("Select error: " + std::string(strerror(errno)));
        }
        return "";
    }
    
    if (activity == 0) {
        // Timeout occurred
        return "";
    }
    
    if (FD_ISSET(sockfd, &readfds)) {
        char buffer[1024];
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        ssize_t bytes_received = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                         (struct sockaddr*)&client_addr, &client_len);
        
        if (bytes_received < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                throw std::runtime_error("Receive error: " + std::string(strerror(errno)));
            }
            return "";
        }
        
        buffer[bytes_received] = '\0';
        return std::string(buffer);
    }
    
    return "";
}

void UDPListener::stop() {
    is_running = false;
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
    }
}
