# Custom C++ HTTP/1.1 Server

A custom HTTP/1.1 web server written in modern C++ from scratch using standard POSIX network sockets (`sys/socket.h`, `netinet/in.h`).

This project is built to deeply understand low-level network programming, the socket API lifecycle, and HTTP protocol mechanics under the hood.

---

## 🛠️ Current Status (Phase 2)

- [x] Basic POSIX TCP socket creation (`socket`, `bind`, `listen`, `accept`)
- [x] Multithreaded request handling using a custom C++ `ThreadPool`
- [x] Thread-safe task queue with `std::mutex` and `std::condition_variable`
- [x] Raw HTTP request logging tagged with worker thread IDs
- [x] Basic HTTP/1.1 200 OK HTML response generation
- [ ] Non-blocking I/O with Event Loop / Reactor pattern (`kqueue`/`epoll`) (Phase 3)
- [ ] Full HTTP request parser (headers, query params, path routing)

---

## 🚀 Building and Running

### Prerequisites
- CMake (>= 3.16) or direct C++17 compiler (`clang++` / `g++`)

### Build Instructions

**Option 1: Using CMake**
```bash
mkdir -p build && cd build
cmake ..
make
```

**Option 2: Direct compilation (`clang++`)**
```bash
mkdir -p build
clang++ -std=c++17 -pthread -Iinclude src/main.cpp src/server.cpp src/thread_pool.cpp -o build/http_server
```

### Run the Server

```bash
./http_server
```

Once running, test it out with:
- **Browser:** Open [http://localhost:8080](http://localhost:8080)
- **cURL:**
  ```bash
  curl -v http://localhost:8080
  ```
