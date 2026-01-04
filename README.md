Minnow – TCP Implementation (Stanford CS144)
This project is an implementation of a TCP-like reliable transport protocol built as part of the Minnow (CS144 – Computer Networking) framework by Stanford University.  
The goal of the project is to understand how TCP works internally by implementing its core components from scratch.

Project Overview:
The implementation follows a layered network stack design, progressively building functionality required for a working TCP connection. The project focuses on correctness, reliability, and protocol behavior rather than performance optimizations.

Key objectives include:
- Reliable, in-order data delivery
- Flow control using sliding windows
- Proper handling of acknowledgments and retransmissions
- Connection management following TCP semantics

Components Implemented:
- *ByteStream*
  - Bounded in-memory byte stream
  - Supports reading, writing, and end-of-input signaling

- *Stream Reassembler*
  - Reassembles out-of-order segments
  - Delivers contiguous data to the ByteStream

- *TCP Receiver*
  - Handles incoming segments
  - Manages acknowledgment numbers and window size

- *TCP Sender*
  - Manages sequence numbers
  - Handles retransmissions and timeouts
  - Implements sliding window flow control

- *TCP Connection*
  - Integrates sender and receiver
  - Manages connection state and clean shutdown

Build Instructions
Prerequisites:
- Linux environment
- CMake
- C++ compiler (g++ or clang)

Build:
```bash
mkdir build
cd build
cmake ..
cmake --build .
