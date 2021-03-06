#ifndef OPPVS_STUN_SERVER_THREAD_HPP
#define OPPVS_STUN_SERVER_THREAD_HPP


#include "thread.hpp"

#include "stun_socket.hpp"
#include "stun_message_handler.hpp"
#include "dynamic_buffer.hpp"
#include <vector>

namespace oppvs
{
	class StunServerThread : public Thread 
	{
	public:
		StunServerThread();
		~StunServerThread();

		int init(StunSocket* sockets, StunTransportAddressSet* stas, SocketRole role);
		int start();
		int run();

		int signalForStop(bool postMessages);
    	int waitForStopAndClose();
	private:
		StunSocket* m_sendSockets;
		int m_rotation;
		std::vector<StunSocket*> m_listenSockets;

		StunTransportAddressSet m_stas;
		bool m_exitThread;

		static void* threadExecuteFunction(void* param);
		StunSocket* waitForSocketData();
		int allocBuffers();
		int releaseBuffers();
		int handleMessage();

		SharedDynamicBufferRef m_incomingBuffer;
		SharedDynamicBufferRef m_outgoingBuffer;

		StunIncomingMessage m_incomingMessage;
		StunOutgoingMessage m_outgoingMessage;

		StunMessageParser m_messageParser;
		SharedDynamicBufferRef m_readerBuffer;
		
	};
}

#endif