#include "server.hpp"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

Server::Server(int port, size_t num_threads) 
    : port(port), server_fd(-1), thread_pool(num_threads) {
    // zero out the sockaddr struct so no garbage values mess us up
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET; // ipv4
    address.sin_addr.s_addr = INADDR_ANY; // bind to all available network interfaces (0.0.0.0)
    address.sin_port = htons(port); // convert port to network byte order (big endian)
}

Server::~Server() {
    if (server_fd >= 0) {
        close(server_fd);
    }
}

void Server::start() {
    // 1. create the main stream socket (ipv4 + tcp)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "error: failed to create socket :(\n";
        return;
    }

    // 2. set reuseaddr so we dont get the annoying "address already in use" error when restarting quickly
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "warning: setsockopt SO_REUSEADDR failed\n";
    }

    // 3. bind our socket fd to the ip and port we set up in the constructor
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "error: bind failed on port " << port << "\n";
        close(server_fd);
        server_fd = -1;
        return;
    }

    // 4. put the socket in passive listening mode, with a small backlog queue
    if (listen(server_fd, 128) < 0) {
        std::cerr << "error: listen failed\n";
        close(server_fd);
        server_fd = -1;
        return;
    }

    std::cout << "🚀 server is listening on http://localhost:" << port << "\n";
    std::cout << "ready to handle concurrent requests via thread pool!\n\n";

    // 5. main server loop - accept connections and delegate to worker pool
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // accept blocks until a new client connects
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            std::cerr << "failed to accept client connection\n";
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        std::cout << "--> [main thread] accepted connection from " << client_ip 
                  << ":" << ntohs(client_addr.sin_port) 
                  << " (socket fd: " << client_fd << ")\n";

        // instead of blocking here, we push the client handling job into our thread pool queue
        thread_pool.enqueue([this, client_fd]() {
            handle_client(client_fd);
        });
    }
}

void Server::handle_client(int client_fd) {
    char buffer[4096];
    std::memset(buffer, 0, sizeof(buffer));

    // read raw bytes from client socket
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        std::cerr << "error reading from client socket " << client_fd << "\n";
        close(client_fd);
        return;
    }

    std::cout << "--- [worker thread " << std::this_thread::get_id() 
              << "] Raw HTTP Request (" << bytes_read << " bytes) ---\n";
    std::cout << buffer << "\n";
    std::cout << "--------------------------------------------------------\n";

    // prepare a simple html response body showing concurrent handling
    std::string html_body = 
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head><title>Custom C++ Web Server</title></head>\n"
        "<body style='font-family: sans-serif; text-align: center; padding-top: 50px;'>\n"
        "  <h1>Hey! It works concurrently! 🚀</h1>\n"
        "  <p>This request was processed asynchronously by a worker thread in our ThreadPool.</p>\n"
        "</body>\n"
        "</html>\n";

    // construct basic HTTP/1.1 response header + body
    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(html_body.length()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" + 
        html_body;

    // send response back to client
    ssize_t bytes_sent = send(client_fd, response.c_str(), response.length(), 0);
    if (bytes_sent < 0) {
        std::cerr << "failed to send response to client socket " << client_fd << "\n";
    }

    // close client connection when done
    close(client_fd);
    std::cout << "<-- [worker thread " << std::this_thread::get_id() 
              << "] response sent & client socket " << client_fd << " closed\n\n";
}
