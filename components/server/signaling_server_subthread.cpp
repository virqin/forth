#include "signaling_server_subthread.hpp"
#include <iostream>

namespace oppvs {
	SignalingServerSubThread::SignalingServerSubThread(): Thread(threadExecuteFunction, this), m_socket(NULL), m_sockfd(-1), m_exitThread(false)
	{

	}

	SignalingServerSubThread::~SignalingServerSubThread()
	{
		releaseBuffers();
	}

	int SignalingServerSubThread::init(PhysicalSocket* socket, int sockfd)
	{
		m_socket = socket;
		m_sockfd = sockfd;
		m_exitThread = false;
		allocBuffers();
		return 0;
	}

	void* SignalingServerSubThread::threadExecuteFunction(void* pthis)
	{
		SignalingServerSubThread* thread = (SignalingServerSubThread*)pthis;
		thread->run();
		return NULL;
	}

	void SignalingServerSubThread::run()
	{
		std::cout << "Run subthread" << std::endl;

		while (!m_exitThread)
		{
			int rcvlen = m_socket->Receive(m_sockfd, m_incomingBuffer->data(), m_incomingBuffer->capacity());

			if (rcvlen <= 0)
			{
				std::cout << "Error in receiving packet" << std::endl;
				break;
			}

			std::cout << "Receive msg from " << m_socket->getRemoteAddress().toString() << " at: " << m_socket->getLocalAddress().toString() << std::endl;
			std::cout << "Bytes: " << rcvlen << std::endl;
			m_incomingBuffer->setSize(rcvlen);
			m_outgoingMessage.sock = m_sockfd;
			m_outgoingMessage.destination = m_socket->getRemoteAddress();
			handleMessage();

		}
		m_socket->Close(m_sockfd);
		
	}

	void SignalingServerSubThread::start()
	{
		create();
	}

	void SignalingServerSubThread::signalForStop()
	{
		m_exitThread = true;
		releaseBuffers();
	}

	void SignalingServerSubThread::waitForStopAndClose()
	{
		m_socket->Close(m_sockfd);
		waitUntilEnding();
		m_socket = NULL;
	}

	void SignalingServerSubThread::allocBuffers()
	{
		m_incomingBuffer = SharedDynamicBufferRef(new DynamicBuffer());
		m_incomingBuffer->setSize(MAX_SIGNALING_MESSAGE_SIZE);
		m_outgoingBuffer = SharedDynamicBufferRef(new DynamicBuffer());
		m_outgoingBuffer->setSize(MAX_SIGNALING_MESSAGE_SIZE);
	
		m_readerBuffer = SharedDynamicBufferRef(new DynamicBuffer());
		m_readerBuffer->setSize(MAX_SIGNALING_MESSAGE_SIZE);
		
		m_messageReader.reset();
		m_messageReader.getStream().attach(m_readerBuffer, true);
	}

	void SignalingServerSubThread::releaseBuffers()
	{
		m_incomingBuffer->reset();
		m_outgoingBuffer->reset();		
	}

	void SignalingServerSubThread::handleMessage()
	{
		m_messageReader.reset();
		m_readerBuffer->setSize(0);
		m_messageReader.getStream().attach(m_readerBuffer, true);

		if (m_messageReader.addBytes(m_incomingBuffer->data(), m_incomingBuffer->size()) < 0)
			return;
		switch (m_messageReader.getMessageType())
		{
			case SignalingStreamRegister:
				if (buildIceRequest() < 0)
					return;
				sendResponse();
				break;
			case SignalingIceResponse:
				std::cout << "Receive Ice Response" << std::endl;
				std::vector<IceCandidate>& candidates = m_messageReader.getIceCandidates();

				for (int i = 0; i < candidates.size(); i++)
				{
			        std::cout << "Candidate: " << candidates[i].component << " "
						  << candidates[i].foundation << " "
						  << candidates[i].priority << " "
						  << candidates[i].ip << " "
						  << candidates[i].protocol << " "
						  << candidates[i].port << " "
						  << candidates[i].type << std::endl;
				}
				break;
		}

		
	}

	int SignalingServerSubThread::buildIceRequest()
	{
		m_messageBuilder.reset();
		if (m_messageBuilder.addMessageType(SignalingIceRequest) < 0)
			return -1;

		if (m_messageBuilder.addStreamKey(m_messageReader.getStreamKey()) < 0)
			return -1;

		return 0;
	}

	void SignalingServerSubThread::sendResponse()
	{
		SharedDynamicBufferRef buffer;
		if (m_messageBuilder.getResult(buffer) < 0)
		{
			std::cout << "Can not build the message to send" << std::endl;
			return;
		}
		
		if (m_socket->Send(m_outgoingMessage.sock, buffer->data(), buffer->size()) >= 0)
			printf("Sent response to %s\n", m_outgoingMessage.destination.toString().c_str());
	}

} // oppvs