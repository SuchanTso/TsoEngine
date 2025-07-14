#include "TPch.h"
#include "NetworkAPI.h"
#include <string>

namespace Tso {
#ifdef TSO_PLATFORM_WINDOWS
#include <winsock2.h>
#include<ws2tcpip.h>

    struct InetAddress {

        InetAddress(const std::string& ip,const uint16_t& port){
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
        sockaddr_in GetSockAddr() const { return addr_; }
        socklen_t GetSockLen() const{ return sizeof(addr_); }

        //bool operator==(const InetAddress& other);

    private:
        sockaddr_in addr_;
    };

    TCPChannel::TCPChannel(): socketFd_(-1), m_Connect(false){}

    TCPChannel::~TCPChannel()
    {
        TSO_CORE_INFO("goodbye TCP");

        Close();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        WSACleanup();
    }

    bool TCPChannel::Connect(const InetAddress& serverAddress)
    {
        socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (socketFd_ == -1) { 
            TSO_CORE_ERROR( "unable to create socket: {}", strerror(errno));
            return false; 
        }

        // 设置非阻塞模式（可选）
        u_long mode = 1;
        ioctlsocket(socketFd_, FIONBIO, &mode);

        if (connect(socketFd_, reinterpret_cast<sockaddr*>(&serverAddress.GetSockAddr()), serverAddress.GetSockLen()) != 0) {
            // 如果连接失败且非阻塞，等待连接完成
            fd_set writeSet;
            FD_ZERO(&writeSet);
            FD_SET(socketFd_, &writeSet);

            timeval timeout = { 5, 0 }; // 5秒超时
            int result = select(socketFd_ + 1, nullptr, &writeSet, nullptr, &timeout);
            if (result <= 0) {
                close(socketFd_);
                socketFd_ = -1;
                return false;
            }
        }

        m_Connect = true;
        remoteAddr_ = CreateRef<InetAddress>(serverAddress);
        localAddr_ = CreateRef<InetAddress>(GetLocalAddress());
        TSO_CORE_INFO("get socket = {}", socketFd_);
        return true;
    }

    bool TCPChannel::Connect(const std::string& ip, const uint16_t& port)
    {
        InetAddress serverAddres(ip, port);
        return Connect(serverAddres);
    }

    InetAddress TCPChannel::GetLocalAddress()const{
        InetAddress addr;
        socklen_t len = sizeof(addr.GetSockAddr());
        getsockname(socketFd_, reinterpret_cast<sockaddr*>(&addr.GetSockAddr()), &len);
        return addr;
    }

    int TCPChannel::ReceiveNonBlocking(void* buffer, size_t length)
    {
        // 设置非阻塞模式
        //fcntl(socketFd_, F_SETFL, O_NONBLOCK); // Linux/macOS
        u_long mode = 1;
        ioctlsocket(socketFd_, FIONBIO, &mode); // Windows

        int received = recv(socketFd_, static_cast<char*>(buffer), length, 0);
        if (received == -1 && errno == EAGAIN) {
            return 0; // 无数据可用
        }
        return received;
    }

    bool TCPChannel::Send(void* data, const size_t& length, const InetAddress& IPAddress) {
        return Send(data, length, *remoteAddr_);
    }

    bool TCPChannel::Send(const void* data, const size_t& length)
    {
        if (!m_Connect || socketFd_ == -1) return false;

        
        const char* buffer = static_cast<const char*>(data);
        TSO_CORE_INFO("send {} byte", length);
        /*for (int i = 0; i < length; ++i) {
            TSO_CORE_INFO("{:#x} ", (unsigned char)buffer[i]);
        }*/
        size_t totalSent = 0;
        while (totalSent < length) {
            size_t sent = send(socketFd_, buffer + totalSent, length - totalSent, 0);
            if (sent <= 0) break;
            totalSent += sent;
        }
        return totalSent == length;
    }

    size_t TCPChannel::Receive(void* buffer, const size_t& size) {
        if (!m_Connect || socketFd_ == -1) return 0;
        size_t received = recv(socketFd_, static_cast<char*>(buffer), size, 0);
        return received > 0 ? static_cast<size_t>(received) : 0;
    }

    void TCPChannel::Close() {
        if (socketFd_ != -1) {
            // 清空接收缓冲区
            char dummyBuffer[1024];
            while (true) {
                int received = recv(socketFd_, dummyBuffer, sizeof(dummyBuffer), 0);
                if (received <= 0) break; // 无数据或错误时退出
            }

            // 再执行正常关闭流程
            shutdown(socketFd_, SD_BOTH);
            closesocket(socketFd_);
            socketFd_ = -1;
        }
        m_Connect = false;
    }


#else
    TSO_ASSERT(false, "NetworkAPI for other platform is not implemented yet")
#endif
}