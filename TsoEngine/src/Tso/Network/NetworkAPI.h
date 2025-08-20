#pragma once
#include <string>
namespace Tso {

    typedef struct InetAddress InetAddress;

	class INetworkChannel {
	public:
		INetworkChannel() = default;
		~INetworkChannel() = default;
        virtual bool Send(void* data, const size_t& length, const InetAddress& IPAddress) = 0;
        virtual size_t Receive(void* buffer , const size_t& size) = 0;
        virtual bool IsConnected() = 0;
	};

    class TCPChannel : public INetworkChannel {
    public:
        TCPChannel();
        ~TCPChannel();
        virtual bool Send(void* data, const size_t& length, const InetAddress& IPAddress) override;
        bool Send(const void* data, const size_t& length);
        virtual size_t Receive(void* buffer, const size_t& size) override;
        virtual bool IsConnected() override { return m_Connect; }
        bool TCPChannel::InitializeWSA();
        bool Connect(const InetAddress& serverAddress);
        bool Connect(const std::string& ip , const uint16_t& port);
        InetAddress GetLocalAddress() const;
        int ReceiveNonBlocking(void* buffer, size_t length);
        void Close();
        std::chrono::steady_clock::time_point& GetLastSend(){return m_LastSendTime;}

    private:
        bool m_Connect = false;
        int socketFd_ = -1;
        Ref<InetAddress> localAddr_ = nullptr;
        Ref<InetAddress> remoteAddr_ = nullptr;
        std::chrono::steady_clock::time_point m_LastSendTime;
    };

    /*class UDPChannel : public INetworkChannel {
    public:
        UDPChannel() = default;
        ~UDPChannel() = default;
        virtual void Send(void* data, const size_t& length, const InetAddress& IPAddress) override;
        virtual size_t Receive(void* buffer, const size_t& size, InetAddress* IPAddress = nullptr) override;
        virtual bool IsConnected() override { return false; }

    private:
        
    };*/


}
