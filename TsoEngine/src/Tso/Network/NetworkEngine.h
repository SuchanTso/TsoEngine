#pragma once
#include<string>
#include "readerwriterqueue.h"
#include "ByteStream.h"
namespace Tso {
	class TCPChannel;
	//class UDPChannel;
    class IProtocolHandler;
	class NetWorkEngine {
	public:
		NetWorkEngine();
		~NetWorkEngine();
		void static Init();
		void static Shutdown();
		bool static Connect(const std::string& ip, const uint16_t& port);
		bool static DisConnect();
		void static RegistryProtocol(const uint8_t& protocolID , const std::function<ByteStream(const ByteStream&)>& wrapFunc);
		void static RegistryProtocol(const std::unordered_map<uint8_t , std::function<ByteStream(const ByteStream&)>>&wrapFuncs);
		void static RegistryRecvFunction(const uint8_t& funcID, const std::function<void(ByteStream&)>& recvFunc);
		bool static HandlerNetwork(const uint8_t& protocolID, ByteStream& byte, bool Reliable = true);
		void static OnUpdate(TimeStep ts);	
		void static RegistryScene(Ref<Scene>scene);
        void static SetContext(Ref<Scene>scene);
		void StartReceiveThread();
		void ProcessBuffer(std::vector<uint8_t>& buffer);
        void HeartBeat(const long long& durationSeconds , const uint8_t& protocolID);
		//void static DefaultWrap(const uint8_t& protocolID , ByteStream& byte);
        
        // Interface for upper Level
        static void Send(uint16_t protocolId, ByteStream& payload);

        // message registry API for upper Level
        static void RegisterHandler(uint8_t moduleId, IProtocolHandler* handler);
        static void UnregisterHandler(uint8_t moduleId);
	private:
		std::unordered_map<uint8_t , std::function<ByteStream(const ByteStream&)>> m_WrapFuncs;
		std::unordered_map<uint8_t, std::function<void(ByteStream&)>> m_RecvFuncs;
		Ref<TCPChannel> m_TCPChannel = nullptr;
		//Ref<UDPChannel> m_UDPChannel = nullptr;
		std::atomic<bool> m_Receiving{ false };
		std::thread m_RecvThread;
		Ref<Scene> m_Scene;
		moodycamel::ReaderWriterQueue<ByteStream> m_PacketQueue; // нчкЬ╤сап
        // --- [NEW]application methods map ---
        std::unordered_map<uint8_t, IProtocolHandler*> m_Handlers;

        // --- [NEW] handler private method ---
        void HandlePacket(ByteStream& stream);
	};
}
