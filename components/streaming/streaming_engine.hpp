/*
	Streaming engine
*/

#ifndef OPPVS_STREAMING_ENGINE_HPP
#define OPPVS_STREAMING_ENGINE_HPP


#include "datatypes.hpp"
#include "thread.hpp"
#include "video_capture.hpp"
#include "audio_engine.hpp"

#include "signaling_handler.hpp"
#include "packet_handler.hpp"
#include "audio_packet_handler.h"

#include "streaming_send_thread.hpp"

#include <vector>

extern "C"
{
	#include "random_unique32.h"
}

namespace oppvs
{

	struct StreamingConfiguration
	{
		IceServerInfo stunServer;
		IceServerInfo turnServer;
		SocketAddress signalingServerAddress;
		StreamingRole role;
	};

	const std::string STUN_SERVER_ADDRESS("192.168.0.100");
	const std::string TURN_SERVER_ADDRESS("192.168.0.100");
	const std::string TURN_SERVER_USER("turn");
	const std::string TURN_SERVER_PASS("password");
	const std::string SIGN_SERVER_ADDRESS("192.168.0.100");
	const static int SIGN_SERVER_PORT = 33333;

	class StreamingEngine
	{
	public:
		StreamingEngine();
		~StreamingEngine();

		int init(StreamingRole role, const std::string& stun, const std::string& turn, 
			const std::string& username, const std::string& password, const std::string& signaling, uint16_t port);
		int start(const std::string& streamkey);

		void setSSRC(uint32_t value) { m_ssrc = value; }
		uint32_t getSSRC() { return m_ssrc;}

		void setStreamInfo(const std::vector<VideoActiveSource>& videoSources, const std::vector<AudioActiveSource>& audioSources);

		bool isRunning();
		void setIsRunning(bool value);
		void registerCallback(frame_callback cb);

		void createSendingThread(IceStream* stream);
		void createMainThread();
		void send();
		void receive(uint8_t* data, uint32_t len);
		void receive();

		void pushData(PixelBuffer& pf);
		void pushData(GenericAudioBufferList& ab);

		int updateStreamInfo(const VideoStreamInfo& info);
	private:
		uint32_t m_ssrc;
		bool m_isRunning;

		uint32_t generateSSRC();

		frame_callback m_callback;

		ServiceInfo m_serviceInfo;	//Store information of stream: video capture sources info
		void printServiceInfo();

		StreamingConfiguration 				m_configuration;
		SignalingHandler 					m_signaler;
		Packetizer 							m_packetizer;
		std::vector<StreamingSendThread*> 	m_sendingThreads;
		Thread*								m_mainThread;	//Thread to distribute segments to all sending threads
		bool								m_exitMainThread;
		tsqueue<SharedDynamicBufferRef> 	m_sendPool;
		Depacketizer						m_depacketizer;
		tsqueue<IncomingStreamingFrame*>	m_recvPool;

		//Audio
		AudioPacketizer						m_audioPacketizer;

		static void* runMainThreadFunction(void* object);
		static void onNewSubscriber(void* object, IceStream* stream);
		static void onReceiveSegment(void* object, uint8_t* data, uint32_t len);

		StreamingRole getRole();
	};

}
#endif