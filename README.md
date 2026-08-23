# BareMetal HTTP

A lightweight, zero-dependency HTTP/1.1 web server built entirely in C++17. Designed to bypass bloated web frameworks, BareMetal interfaces directly with the OS network stack via raw POSIX sockets, making it ideal for resource-constrained embedded systems and IoT devices.

---

## Core Architecture & Features

* **Raw POSIX Sockets:** Direct `sys/socket.h` and `netinet/in.h` memory and connection management without heavy external dependencies (no Boost, no POCO).
* **Custom Concurrency Engine:** Utilizes a custom C++ Thread Pool (`std::mutex`, `std::condition_variable`) to handle concurrent client connections efficiently, avoiding the massive CPU overhead of per-request thread allocation.
* **Zero-Dependency HTTP Parser:** A custom string-parsing engine designed to decode raw TCP network buffers, extract HTTP methods, and map header key-value pairs natively.
* **URI Router & File Serving:** Dynamically routes HTTP requests and streams static disk assets (HTML, text) directly to the client with accurate MIME typing and HTTP status codes.

---

## Project Structure

```text
reactor-http-server/
├── CMakeLists.txt
├── include/
│   ├── server.hpp         # POSIX socket lifecycle (bind, listen, accept)
│   ├── thread_pool.hpp    # Concurrency primitives and worker queue
│   ├── http_request.hpp   # Raw buffer parsing and header extraction
│   └── router.hpp         # URI mapping and static file serving
├── src/
│   ├── main.cpp           # Entry point (port 8080)
│   ├── server.cpp         
│   ├── thread_pool.cpp    
│   ├── http_request.cpp   
│   └── router.cpp         
└── www/
    └── index.html         # Test static asset
