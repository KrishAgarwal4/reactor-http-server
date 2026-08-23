#include "server.hpp"
#include <iostream>

int main() {
    const int PORT = 8080;

    std::cout << "Starting custom HTTP server project...\n";

    Server server(PORT);
    server.start();

    return 0;
}
