#include"TPch.h"
#include "NetworkEngine.h"
#include "NetworkAPI.h"
#include "ByteStream.h"
#include "thread"
#include "IProtocol.h"
namespace Tso {
	//void NetWorkEngine::TestSend(const std::string& ip, const uint16_t& port, char* msg)
	//{
	//	Ref<TCPChannel> tcp = CreateRef<TCPChannel>();
	//	tcp->Connect(ip, port);
	//	tcp->Send((void*)msg , strlen(msg) + 1);
	//	//tcp->Close();
	//}
	static NetWorkEngine* s_NetworkEngine = nullptr;
	NetWorkEngine::NetWorkEngine()
	{
		m_TCPChannel = CreateRef<TCPChannel>();
		//m_UDPChannel = CreateRef<UDPChannel>();
	}
	void NetWorkEngine::Init()
	{
		TSO_CORE_ASSERT(s_NetworkEngine == nullptr, "Init NetworkEngine more than once!");
		s_NetworkEngine = new NetWorkEngine();
		

		s_NetworkEngine->StartReceiveThread();
		
	}
	void NetWorkEngine::Shutdown()
	{

		delete s_NetworkEngine;
		s_NetworkEngine = nullptr;
	}

	bool NetWorkEngine::Connect(const std::string& ip, const uint16_t& port)
	{
		TSO_CORE_ASSERT(s_NetworkEngine != nullptr, "Didn't initialised networkEngine yet");
		return s_NetworkEngine->m_TCPChannel->Connect(ip, port);
	}

	bool NetWorkEngine::DisConnect()
	{
		TSO_CORE_ASSERT(s_NetworkEngine != nullptr, "Didn't initialised networkEngine yet");
		if (s_NetworkEngine->m_TCPChannel->IsConnected()) {
			s_NetworkEngine->m_TCPChannel->Close();
			return true;
		}
		else {
			return false;
		}
		
	}

	void NetWorkEngine::RegistryProtocol(const uint8_t& protocolID , const std::function<ByteStream(const ByteStream&)>& wrapFunc)
	{	
		TSO_CORE_ASSERT(s_NetworkEngine != nullptr, "Didn't initialised networkEngine yet");
		TSO_CORE_ASSERT(s_NetworkEngine->m_WrapFuncs.find(protocolID) == s_NetworkEngine->m_WrapFuncs.end(), "already registed {} for ProtocolID", protocolID);
		s_NetworkEngine->m_WrapFuncs[protocolID] = wrapFunc;
		
	}

	void NetWorkEngine::RegistryProtocol(const std::unordered_map<uint8_t, std::function<ByteStream(const ByteStream&)>>& wrapFuncs)
	{
		TSO_CORE_ASSERT(s_NetworkEngine != nullptr, "Didn't initialised networkEngine yet");
		s_NetworkEngine->m_WrapFuncs = wrapFuncs;
	}

	void NetWorkEngine::RegistryRecvFunction(const uint8_t& funcID, const std::function<void(ByteStream&)>& recvFunc)
	{
		TSO_CORE_ASSERT(s_NetworkEngine != nullptr, "Didn't initialised networkEngine yet");
		TSO_CORE_ASSERT(s_NetworkEngine->m_RecvFuncs.find(funcID) == s_NetworkEngine->m_RecvFuncs.end(), "already registed {} for funcID", funcID);
		s_NetworkEngine->m_RecvFuncs[funcID] = recvFunc;
	}

	bool NetWorkEngine::HandlerNetwork(const uint8_t& protocolID, ByteStream& byte, bool Reliable)
    {
        TSO_CORE_ASSERT(s_NetworkEngine != nullptr, "Didn't initialised networkEngine yet");
        
        // no need for wrap funcs, ClientNetworkManager will build payload completely.
        // ByteStream wrappedByte = s_NetworkEngine->m_WrapFuncs[protocolID](byte);
        
        size_t payloadSize = byte.getRawBufferLength();
        uint32_t totalSize = ByteStream::HEADER_SIZE + payloadSize;
        
        std::vector<uint8_t> sendBuffer;
        sendBuffer.resize(totalSize);

        // total length
        memcpy(sendBuffer.data(), &totalSize, ByteStream::PACK_LENGTH);
        // protocol ID
        memcpy(sendBuffer.data() + ByteStream::PACK_LENGTH, &protocolID, ByteStream::PACK_LENGTH);
        // copy Payload
        if (payloadSize > 0) {
            memcpy(sendBuffer.data() + ByteStream::HEADER_SIZE, byte.getRawBuffer(), payloadSize);
        }

        bool res = false;
        if (Reliable && s_NetworkEngine->m_TCPChannel->IsConnected()) {
            res = s_NetworkEngine->m_TCPChannel->Send(sendBuffer.data(), totalSize);
            if (res) {
                // TSO_CORE_INFO("Send successfully");
            } else {
                TSO_CORE_ERROR("Unable to send");
            }
        }
        return res;
    }

    // [MODIFIED] static Send method for upper level
    void NetWorkEngine::Send(uint16_t protocolId, ByteStream& payload) {
        TSO_CORE_ASSERT(s_NetworkEngine != nullptr, "NetworkEngine not initialized");
        if (!s_NetworkEngine->m_TCPChannel || !s_NetworkEngine->m_TCPChannel->IsConnected()) {
            TSO_CORE_WARN("Not connected. Cannot send message.");
            return;
        }

        size_t payloadSize = payload.getRawBufferLength();
        uint32_t totalSize = ByteStream::HEADER_SIZE + payloadSize;
        std::vector<uint8_t> sendBuffer(totalSize);

        memcpy(sendBuffer.data(), &totalSize, ByteStream::PACK_LENGTH);
        memcpy(sendBuffer.data() + ByteStream::PACK_LENGTH, &protocolId, ByteStream::PROTOCOL_SIZE);
        if (payloadSize > 0) {
            memcpy(sendBuffer.data() + ByteStream::HEADER_SIZE , payload.getRawBuffer(), payloadSize);
        }

        s_NetworkEngine->m_TCPChannel->Send(sendBuffer.data(), totalSize);
    }

    void NetWorkEngine::RegisterHandler(uint8_t moduleId, IProtocolHandler* handler) {
        TSO_CORE_ASSERT(s_NetworkEngine != nullptr, "NetworkEngine not initialized");
        if (s_NetworkEngine->m_Handlers.count(moduleId)) {
            TSO_CORE_WARN("Handler for module ID %u overwritten.", moduleId);
        }
        s_NetworkEngine->m_Handlers[moduleId] = handler;
    }

    void NetWorkEngine::UnregisterHandler(uint8_t moduleId) {
        TSO_CORE_ASSERT(s_NetworkEngine != nullptr, "NetworkEngine not initialized");
        s_NetworkEngine->m_Handlers.erase(moduleId);
    }


	void NetWorkEngine::OnUpdate(TimeStep ts)
	{
		if (s_NetworkEngine->m_TCPChannel->IsConnected()) {
			//TODO(Suchan): heartbeat
            s_NetworkEngine->HeartBeat(28, 255);
		}
		// 每帧最多处理10个包防止卡顿
		constexpr int MAX_PACKETS_PER_FRAME = 10;
		int processed = 0;

        ByteStream packet;
		while (processed++ < MAX_PACKETS_PER_FRAME &&
			s_NetworkEngine->m_PacketQueue.try_dequeue(packet)) {
            s_NetworkEngine->HandlePacket(packet);
		}
	}

    void NetWorkEngine::HandlePacket(ByteStream& stream) {
        if (stream.getRawBufferLength() < ByteStream::PROTOCOL_SIZE) {
            TSO_CORE_ERROR("Received packet is too small.");
            return;
        }

        uint16_t protocolId = stream.read<uint16_t>();
        uint8_t moduleId = (protocolId >> 8) & 0xFF;
        uint8_t commandId = protocolId & 0xFF;

        auto it = m_Handlers.find(moduleId);
        if (it != m_Handlers.end() && it->second != nullptr) {
            it->second->HandleNetworkMessage(commandId, stream);
        } else {
            TSO_CORE_WARN("No handler for module ID: %u", moduleId);
        }
    }

	void NetWorkEngine::RegistryScene(Ref<Scene> scene)
	{
		TSO_CORE_ASSERT(s_NetworkEngine != nullptr, "Didn't initialised networkEngine yet");
		s_NetworkEngine->m_Scene = scene;
		auto rawSend = [](const ByteStream& byte) ->ByteStream {
			return byte;
			};
		s_NetworkEngine->RegistryProtocol(0, rawSend);
		auto rawRecv = [&](ByteStream& byte) ->void {
			uint8_t protocol = byte.read<uint8_t>();
			size_t dataLength = byte.read<size_t>();
			byte.DeseriealizeEntity(s_NetworkEngine->m_Scene);
			};
		s_NetworkEngine->RegistryRecvFunction(0 , rawRecv);
	}

    void  NetWorkEngine::SetContext(Ref<Scene>scene){
        s_NetworkEngine->m_Scene = scene;
    }


	void NetWorkEngine::StartReceiveThread()
	{
		m_Receiving = true;
		m_RecvThread = std::thread([this]() {
			constexpr size_t CHUNK_SIZE = 4096;
			std::vector<uint8_t> tempBuffer;

			while (m_Receiving) {
				// 非阻塞读取
				char chunk[CHUNK_SIZE];
				int received = m_TCPChannel->ReceiveNonBlocking(chunk, CHUNK_SIZE);

				if (received > 0) {
					tempBuffer.insert(tempBuffer.end(), chunk, chunk + received);
					ProcessBuffer(tempBuffer); // 处理缓冲区
				}
				else if (received == 0) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 避免忙等待
				}
			}
			});
	}

	void NetWorkEngine::ProcessBuffer(std::vector<uint8_t>& buffer)
	{
        while (buffer.size() >= ByteStream::PACK_LENGTH) {
            uint32_t totalPacketSize = 0;
            memcpy(&totalPacketSize, buffer.data(), ByteStream::PACK_LENGTH);

            if (buffer.size() < totalPacketSize) {
                // incomplete data
                break;
            }
            
            
            // report the whole pack to application level after parsing the total length of the pack
            ByteStream packet_payload(buffer.data() + ByteStream::PACK_LENGTH, totalPacketSize - ByteStream::PACK_LENGTH);
            
            // funcID for handler function ClientNetworkManager
            m_PacketQueue.enqueue(std::move(packet_payload));
            
            // remove dealt data
            buffer.erase(buffer.begin(), buffer.begin() + totalPacketSize);
        }
	}

    void NetWorkEngine::HeartBeat(const long long& durationSeconds , const uint8_t& protocolID){
        if(m_TCPChannel && m_TCPChannel->IsConnected()){
            auto now = std::chrono::steady_clock::now();
            if(std::chrono::duration_cast<std::chrono::seconds>(now - m_TCPChannel->GetLastSend()) > std::chrono::seconds(durationSeconds)){
                std::vector<uint8_t> content = {255};
                ByteStream heartByte(content);
                ByteStream::PackHeader(heartByte, {protocolID , heartByte.getRawBufferLength() + ByteStream::HEADER_SIZE});
                m_TCPChannel->Send(heartByte.getRawBuffer(), heartByte.getRawBufferLength());
                TSO_CORE_INFO("heart beating");
            }
        }
    }


	

	

	NetWorkEngine::~NetWorkEngine() {
		m_Receiving = false;
		// 先关闭连接，确保接收线程退出
		if (m_TCPChannel && m_TCPChannel->IsConnected()) {
			m_TCPChannel->Close();  // 确保发送FIN包
			m_TCPChannel = nullptr;
		}
		// 再等待线程结束
		if (m_RecvThread.joinable()) {
			m_RecvThread.join();
		}
		
	}



}
