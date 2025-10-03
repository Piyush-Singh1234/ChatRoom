# Asynchronous Chat Server (C++ & Boost.Asio)

## Project Overview

This is a high-performance, concurrent chat application built using **C++** and the **Boost.Asio** library. The server employs a non-blocking, asynchronous architecture to efficiently handle multiple simultaneous client connections, ensuring responsiveness and scalability.

The server's design focuses on isolating client state and utilizing a single-threaded event loop (`io_context`) to manage all network I/O operations, thereby eliminating common multi-threading synchronization issues.

## Features

  * **Asynchronous I/O:** Utilizes `boost::asio::async_read` and `async_write` for non-blocking network operations, allowing the server to handle dozens of clients without lag.
  * **Scalable Architecture:** Implemented a robust **Room / Session / Participant** design pattern, where each connected client is managed by its own dedicated `Session` object.
  * **Safe Resource Management:** Employs **`std::shared_ptr`** and **`std::enable_shared_from_this`** to manage the lifecycle of `Session` objects, guaranteeing they remain active for the duration of pending asynchronous operations.
  * **Thread-Safe Communication:** Uses a separate `std::thread` for client-side user input, scheduling all network write operations onto the `io_context` via `boost::asio::post` to prevent race conditions.
  * **Message Queues:** Implements an outgoing message queue within each `Session` to serialize write operations, ensuring messages are delivered in the correct order to the client.

## Prerequisites

To build and run this project, you need:

1.  A modern C++ compiler (g++ recommended).
2.  The **Boost C++ Libraries**, specifically **Boost.Asio**.

### Installing Boost (Linux/macOS)

```bash
# On Debian/Ubuntu
sudo apt-get install libboost-all-dev

# On macOS (using Homebrew)
brew install boost
```

## Build and Run

Assuming your source files are named `server.cpp` (or equivalent) and `client.cpp`.

### 1\. Build the Executables

Compile the server and client, linking against the Boost libraries:

```bash
# Compile the Server
g++ -std=c++17 server.cpp chatRoom.cpp message.cpp -o chatApp -lboost_system -pthread

# Compile the Client
g++ -std=c++17 client.cpp message.cpp -o clientApp -lboost_system -pthread
```

### 2\. Run the Server

Start the chat server on a specific port (e.g., 8080):

```bash
./chatApp 8080
```

The server will now be listening for incoming connections.

### 3\. Run the Client(s)

Open multiple terminal windows and connect clients to the server's IP (use `127.0.0.1` for local testing) and port:

```bash
# Client 1
./clientApp 8080

# Client 2
./clientApp 8080
```

You can now start typing messages in the client windows, and the server will broadcast them to all other connected participants.

## Architecture Overview

The server's design is based on the following key components:

| Component | Role | Key Function |
| :--- | :--- | :--- |
| **`io_context`** | The central asynchronous engine. | Manages all non-blocking I/O and schedules callbacks. |
| **`Room`** | The chat coordinator. | Maintains a set of all active `Participant`s and broadcasts messages to everyone but the sender. |
| **`Session`** | Manages a single client connection. | Owns the client's unique socket, runs the `async_read` loop, and manages the outgoing message queue. |
| **`acceptor`** | The initial listener. | Runs the `async_accept` loop to wait for new client connections and hands off a new socket to a `Session`. |
| **`Message`** | Data encapsulation. | Handles message formatting, including a custom header protocol for reliable transmission. |
