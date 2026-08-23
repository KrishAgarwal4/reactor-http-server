#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <netinet/in.h>

class Server {
public:
    // constructor takes port number to listen on
    Server(int port);
    ~Server();

    // starts the main listening loop
    void start();

private:
    int port;
    int server_fd;
    struct sockaddr_in address;

    void handle_client(int client_fd);
};

#endif // SERVER_HPP
