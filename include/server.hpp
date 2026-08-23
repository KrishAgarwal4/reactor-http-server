#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <netinet/in.h>
#include "thread_pool.hpp"

class Server {
public:
    // constructor takes port number and optional thread pool size
    Server(int port, size_t num_threads = 4);
    ~Server();

    // starts the main listening loop
    void start();

private:
    int port;
    int server_fd;
    struct sockaddr_in address;

    // thread pool instance to handle client requests concurrently
    ThreadPool thread_pool;

    void handle_client(int client_fd);
};

#endif // SERVER_HPP
