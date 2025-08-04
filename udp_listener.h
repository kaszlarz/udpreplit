#ifndef UDP_LISTENER_H
#define UDP_LISTENER_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

class UDPListener {
private:
    int sockfd;
    struct sockaddr_in server_addr;
    bool is_running;
    
public:
    UDPListener(int port);
    ~UDPListener();
    
    std::string receiveData(int timeout_ms = 0);
    void stop();
    bool isRunning() const { return is_running; }
};

#endif // UDP_LISTENER_H
