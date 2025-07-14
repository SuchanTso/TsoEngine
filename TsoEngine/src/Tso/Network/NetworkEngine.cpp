#include"TPch.h"
#include "NetworkEngine.h"
#include "NetworkAPI.h"
#include "ByteStream.h"
#include "thread"
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

	bool NetWorkEngine::HandlerNetwork(const uint8_t& protocolID, const ByteStream& byte, bool Reliable)
	{
		TSO_CORE_ASSERT(s_NetworkEngine != nullptr, "Didn't initialised networkEngine yet");
		TSO_CORE_ASSERT(s_NetworkEngine->m_WrapFuncs.find(protocolID) != s_NetworkEngine->m_WrapFuncs.end(), "Didn't find protocol [{}] , regist it first!", protocolID);
		ByteStream wrappedByte = s_NetworkEngine->m_WrapFuncs[protocolID](byte);
		bool res = false;
		if (Reliable && s_NetworkEngine->m_TCPChannel->IsConnected()) {
			size_t dataLength = wrappedByte.getRawBufferLength();
			res = s_NetworkEngine->m_TCPChannel->Send(wrappedByte.getRawBuffer(), dataLength);
			if (res) {
				TSO_CORE_INFO("Send successfully");
			}
			else {
				TSO_CORE_INFO("Unable to send");
			}
			
		}
		return res;
	}

	//void static ParseCallback(ByteStream& byte) {
	//	//TSO_CORE_TRACE("tagLength:{}", byte.read<uint32_t>());
	//	TSO_CORE_TRACE("tag:{}", byte.readString());
	//	for(int i = 0 ; i < 3 ; i++) TSO_CORE_TRACE("translation[i]:{}", byte.read<float>());
	//	for (int i = 0; i < 3; i++) TSO_CORE_TRACE("scale[i]:{}", byte.read<float>());
	//	for (int i = 0; i < 3; i++) TSO_CORE_TRACE("rotation[i]:{}", byte.read<float>());
	//	TSO_CORE_TRACE("ID:{}", byte.read<uint64_t>());
	//}

	void NetWorkEngine::OnUpdate(TimeStep ts)
	{
		if (s_NetworkEngine->m_TCPChannel->IsConnected()) {
			//TODO(Suchan): heartbeat
		}
		// 每帧最多处理10个包防止卡顿
		constexpr int MAX_PACKETS_PER_FRAME = 10;
		int processed = 0;

		std::pair<uint8_t , ByteStream> packet;
		while (processed++ < MAX_PACKETS_PER_FRAME &&
			s_NetworkEngine->m_PacketQueue.try_dequeue(packet)) {
			//ParseCallback(packet.second);
			if (s_NetworkEngine->m_RecvFuncs.find(packet.first) != s_NetworkEngine->m_RecvFuncs.end()) {
				s_NetworkEngine->m_RecvFuncs[packet.first](packet.second);
			}
			else {
				TSO_CORE_ERROR("Received unknown protocol message [{}] , dropped it", packet.first);
			}
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
		while (buffer.size() >= ByteStream::HEADER_SIZE) {
			ByteStream byte = ByteStream(buffer);
			uint8_t protocol = byte.read<uint8_t>();
			size_t dataLength = byte.read<size_t>();
			const size_t packetSize = byte.getRawBufferLength();

			if (dataLength > packetSize) break;// header says there is more data for this byte
			byte.ResetRead();
			m_PacketQueue.enqueue({ protocol ,std::move(byte) });

			// 移除已处理数据
			buffer.erase(buffer.begin(), buffer.begin() + packetSize);
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