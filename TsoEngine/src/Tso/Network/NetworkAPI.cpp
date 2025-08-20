#include "TPch.h"
#include "NetworkAPI.h"
#include <string>
#include <cstring> // For std::memset


#ifdef TSO_PLATFORM_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

#elif defined(TSO_PLATFORM_MACOSX) // Or any other POSIX system like Linux

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <sys/select.h>
#include <sys/time.h>
#endif
namespace Tso {

// Common InetAddress implementation for both platforms
struct InetAddress {
public:
    InetAddress(const std::string& ip, const uint16_t& port) {
        std::memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
    }

    InetAddress() {
        std::memset(&addr_, 0, sizeof(addr_));
    }

    std::string GetIP() const {
        char buffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr_.sin_addr, buffer, INET_ADDRSTRLEN);
        return std::string(buffer);
    }

    uint16_t GetPort() const { return ntohs(addr_.sin_port); }

    // For const access (e.g., connect)
    const sockaddr_in& GetSockAddr() const { return addr_; }

    // For non-const access (e.g., getsockname)
    sockaddr_in* GetSockAddrPtr() { return &addr_; }

    socklen_t GetSockLen() const { return sizeof(addr_); }

private:
    sockaddr_in addr_;
};


#ifdef TSO_PLATFORM_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

    TCPChannel::TCPChannel(): socketFd_(INVALID_SOCKET), m_Connect(false){}

    TCPChannel::~TCPChannel()
    {
        TSO_CORE_INFO("goodbye TCP");
        Close();
        // No need for sleep, WSACleanup will handle necessary cleanup.
        WSACleanup();
    }
    
    // Helper to initialize WSA
    bool TCPChannel::InitializeWSA() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            TSO_CORE_ERROR("WSAStartup failed.");
            return false;
        }
        return true;
    }

    int TCPChannel::ReceiveNonBlocking(void* buffer, size_t length) {
        if (!m_Connect || socketFd_ == INVALID_SOCKET) {
            // It's better to return an error code if the socket is not valid
            return SOCKET_ERROR;
        }

        // The socket is already set to non-blocking in the Connect() method.
        // We need to cast length to int for the recv function signature.
        int received = recv(socketFd_, static_cast<char*>(buffer), static_cast<int>(length), 0);

        if (received == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK) {
                // This is the expected "error" for a non-blocking socket when there is no data.
                // It's not a real error, so we return 0 to indicate "0 bytes received".
                return 0;
            }
            else {
                // This is a real network error.
                TSO_CORE_ERROR("ReceiveNonBlocking failed with a real error: {}", error);
                return SOCKET_ERROR; // Return -1 to signal a fatal error.
            }
        }

        // if received == 0, the peer has performed an orderly shutdown.
        // if received > 0, it's the number of bytes received.
        // In both cases, returning the value from recv is the correct behavior.
        return received;
    }

    bool TCPChannel::Connect(const InetAddress& serverAddress)
    {
        // Initialize WSA for each connection attempt for simplicity, or manage it globally
        if (!InitializeWSA()) return false;
        
        socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (socketFd_ == INVALID_SOCKET) {
            TSO_CORE_ERROR( "unable to create socket: {}", WSAGetLastError());
            return false;
        }

        // Set to non-blocking mode
        u_long mode = 1;
        ioctlsocket(socketFd_, FIONBIO, &mode);

        if (connect(socketFd_, reinterpret_cast<const sockaddr*>(&serverAddress.GetSockAddr()), serverAddress.GetSockLen()) == SOCKET_ERROR) {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                TSO_CORE_ERROR("connect failed with error: {}", WSAGetLastError());
                closesocket(socketFd_);
                socketFd_ = INVALID_SOCKET;
                return false;
            }

            // If connection is pending in non-blocking mode, wait for it to complete
            fd_set writeSet;
            FD_ZERO(&writeSet);
            FD_SET(socketFd_, &writeSet);

            timeval timeout = { 5, 0 }; // 5 second timeout
            int result = select(0, nullptr, &writeSet, nullptr, &timeout); // nfds is ignored on Windows
            if (result <= 0) {
                TSO_CORE_ERROR("select failed or timed out during connect");
                closesocket(socketFd_);
                socketFd_ = INVALID_SOCKET;
                return false;
            }
        }

        m_Connect = true;
        remoteAddr_ = CreateRef<InetAddress>(serverAddress);
        localAddr_ = CreateRef<InetAddress>(GetLocalAddress());
        TSO_CORE_INFO("get socket = {}", socketFd_);
        return true;
    }

    InetAddress TCPChannel::GetLocalAddress()const{
        InetAddress addr;
        socklen_t len = sizeof(sockaddr_in);
        getsockname(socketFd_, reinterpret_cast<sockaddr*>(addr.GetSockAddrPtr()), &len);
        return addr;
    }

    bool TCPChannel::Send(const void* data, const size_t& length)
    {
        if (!m_Connect || socketFd_ == INVALID_SOCKET) return false;

        const char* buffer = static_cast<const char*>(data);
        TSO_CORE_INFO("send {} byte", length);
        
        size_t totalSent = 0;
        while (totalSent < length) {
            int sent = send(socketFd_, buffer + totalSent, length - totalSent, 0);
            if (sent == SOCKET_ERROR) {
                TSO_CORE_ERROR("send failed with error: {}", WSAGetLastError());
                break;
            }
            totalSent += sent;
        }
        return totalSent == length;
    }

    size_t TCPChannel::Receive(void* buffer, const size_t& size) {
        if (!m_Connect || socketFd_ == INVALID_SOCKET) return 0;
        int received = recv(socketFd_, static_cast<char*>(buffer), size, 0);
        return received > 0 ? static_cast<size_t>(received) : 0;
    }

    void TCPChannel::Close() {
        if (socketFd_ != INVALID_SOCKET) {
            // Graceful shutdown
            shutdown(socketFd_, SD_BOTH);
            closesocket(socketFd_);
            socketFd_ = INVALID_SOCKET;
        }
        m_Connect = false;
    }
    
    // Other functions (unchanged logic)
    bool TCPChannel::Connect(const std::string& ip, const uint16_t& port)
    {
        InetAddress serverAddress(ip, port);
        return Connect(serverAddress);
    }
    
    bool TCPChannel::Send(void* data, const size_t& length, const InetAddress& IPAddress) {
        return Send(data, length);
    }


#elif defined(TSO_PLATFORM_MACOSX) // Or any other POSIX system like Linux

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <sys/select.h>
#include <sys/time.h>

    TCPChannel::TCPChannel(): socketFd_(-1), m_Connect(false) {}

    TCPChannel::~TCPChannel()
    {
        TSO_CORE_INFO("goodbye TCP");
        Close();
        // No global cleanup like WSACleanup() is needed on macOS/POSIX
    }

    bool TCPChannel::Connect(const InetAddress& serverAddress)
    {
        socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (socketFd_ == -1) {
            TSO_CORE_ERROR("unable to create socket: {}", strerror(errno));
            return false;
        }

        // Set to non-blocking mode
        int flags = fcntl(socketFd_, F_GETFL, 0);
        fcntl(socketFd_, F_SETFL, flags | O_NONBLOCK);

        if (connect(socketFd_, reinterpret_cast<const sockaddr*>(&serverAddress.GetSockAddr()), serverAddress.GetSockLen()) == -1) {
            if (errno != EINPROGRESS) {
                // A real error occurred
                TSO_CORE_ERROR("connect failed with error: {}", strerror(errno));
                close(socketFd_);
                socketFd_ = -1;
                return false;
            }
            
            // Connection is in progress, wait for it to complete using select()
            fd_set writeSet;
            FD_ZERO(&writeSet);
            FD_SET(socketFd_, &writeSet);

            timeval timeout = { 5, 0 }; // 5 second timeout
            int result = select(socketFd_ + 1, nullptr, &writeSet, nullptr, &timeout);
            
            if (result <= 0) {
                // select() failed or timed out
                if (result == 0) {
                     TSO_CORE_ERROR("connect timed out");
                } else {
                     TSO_CORE_ERROR("select failed during connect: {}", strerror(errno));
                }
                close(socketFd_);
                socketFd_ = -1;
                return false;
            }

            // The socket is now writable, but we should check for socket errors
            int optval;
            socklen_t optlen = sizeof(optval);
            if (getsockopt(socketFd_, SOL_SOCKET, SO_ERROR, &optval, &optlen) == -1 || optval != 0) {
                TSO_CORE_ERROR("connect failed after select: {}", strerror(optval == 0 ? errno : optval));
                close(socketFd_);
                socketFd_ = -1;
                return false;
            }
        }
        
        // Restore blocking mode if desired, but for games non-blocking is often better
        // fcntl(socketFd_, F_SETFL, flags);

        m_Connect = true;
        remoteAddr_ = CreateRef<InetAddress>(serverAddress);
        localAddr_ = CreateRef<InetAddress>(GetLocalAddress());
        TSO_CORE_INFO("get socket = {}", socketFd_);
        return true;
    }

    bool TCPChannel::Connect(const std::string& ip, const uint16_t& port)
    {
        InetAddress serverAddress(ip, port);
        return Connect(serverAddress);
    }

    InetAddress TCPChannel::GetLocalAddress()const{
        InetAddress addr;
        socklen_t len = sizeof(sockaddr_in);
        // Use the non-const pointer getter for getsockname
        getsockname(socketFd_, reinterpret_cast<sockaddr*>(addr.GetSockAddrPtr()), &len);
        return addr;
    }

    int TCPChannel::ReceiveNonBlocking(void* buffer, size_t length)
    {
        // The socket is already non-blocking from the Connect() method.
        // No need to set it again unless you changed it.
        int received = recv(socketFd_, static_cast<char*>(buffer), length, 0);
        if (received == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            return 0; // No data available to read
        }
        return received;
    }

    bool TCPChannel::Send(void* data, const size_t& length, const InetAddress& IPAddress) {
        // Assuming this should always send to the connected remote address
        return Send(data, length);
    }

    bool TCPChannel::Send(const void* data, const size_t& length)
    {
        if (!m_Connect || socketFd_ == -1) return false;
        
        const char* buffer = static_cast<const char*>(data);
        TSO_CORE_INFO("send {} byte", length);

        size_t totalSent = 0;
        while (totalSent < length) {
            ssize_t sent = send(socketFd_, buffer + totalSent, length - totalSent, 0);
            if (sent <= 0) {
                TSO_CORE_ERROR("send failed with error: {}", strerror(errno));
                break;
            }
            totalSent += sent;
        }
        m_LastSendTime = std::chrono::steady_clock::now();
        return totalSent == length;
    }

    size_t TCPChannel::Receive(void* buffer, const size_t& size) {
        if (!m_Connect || socketFd_ == -1) return 0;
        // The socket is in non-blocking mode, so this will behave like a non-blocking recv.
        // If you need a truly blocking receive, you must change socket flags first.
        ssize_t received = recv(socketFd_, static_cast<char*>(buffer), size, 0);
        return received > 0 ? static_cast<size_t>(received) : 0;
    }

    void TCPChannel::Close() {
        if (socketFd_ != -1) {
            // Optional: Drain the receive buffer to allow for a more graceful close
            // This is generally good practice but can be omitted.
            char dummyBuffer[1024];
            // Set socket to non-blocking to drain without getting stuck
            int flags = fcntl(socketFd_, F_GETFL, 0);
            fcntl(socketFd_, F_SETFL, flags | O_NONBLOCK);
            while (recv(socketFd_, dummyBuffer, sizeof(dummyBuffer), 0) > 0);

            // Gracefully shutdown the connection
            shutdown(socketFd_, SHUT_RDWR);
            
            // Close the file descriptor
            close(socketFd_);
            socketFd_ = -1;
        }
        m_Connect = false;
    }


#else
    TSO_ASSERT(false, "NetworkAPI for other platform is not implemented yet")
#endif
}
