#include "message_handling.hpp"
#include <string.h>
#include "error.hpp"

namespace oppvs
{
	Message::Message(): m_length(0)
	{

	}

	Message::~Message()
	{
		m_length = 0;
	}

	void Message::setFlag(uint8_t flag)
	{
		m_data[0] = flag;
	}

	uint8_t Message::getFlag()
	{
		return m_data[0];
	}

	void Message::setSource(uint8_t sid)
	{
		m_data[1] = sid;
	}

	uint8_t Message::getSource()
	{
		return m_data[1];
	}

	void Message::setSegID(uint16_t seg)
	{
		memcpy(m_data + 2, &seg, 2);
	}

	uint16_t Message::getSegID()
	{
		uint16_t seg = 0;
		memcpy(&seg, m_data + 2, 2);
		return seg;
	}

	void Message::setVP8Required(uint8_t req)
	{
		memcpy(m_data + 4, &req, 1);
	}
		
	uint8_t Message::getVP8Required()
	{
		uint8_t req = 0;
		memcpy(&req, m_data + 4, 1);
		return req;
	}
		
	void Message::setVP8OptX(uint8_t optx)
	{
		memcpy(m_data + 5, &optx, 1);
	}

	uint8_t Message::getVP8OptX()
	{
		uint8_t optx = 0;
		memcpy(&optx, m_data + 5, 1);
		return optx;
	}
		
	void Message::setVP8OptY(uint8_t opty)
	{
		memcpy(m_data + 6, &opty, 1);
	}
		
	uint8_t Message::getVP8OptY()
	{
		uint8_t opty = 0;
		memcpy(&opty, m_data + 6, 1);
		return opty;
	}

	void Message::setSize0(uint8_t size0)
	{
		memcpy(m_data + 7, &size0, 1);
	}
		
	uint8_t Message::getSize0()
	{
		uint8_t size0 = 0;
		memcpy(&size0, m_data + 7, 1);
		return size0;
	}
		
	void Message::setSize1(uint8_t size1)
	{
		memcpy(m_data + 8, &size1, 1);
	}

	uint8_t Message::getSize1()
	{
		uint8_t size1 = 0;
		memcpy(&size1, m_data + 8, 1);
		return size1;
	}
		
	void Message::setSize2(uint8_t size2)
	{
		memcpy(m_data + 9, &size2, 1);
	}
		
	uint8_t Message::getSize2()
	{
		uint8_t size2 = 0;
		memcpy(&size2, m_data + 9, 1);
		return size2;
	}

	void Message::setData(const uint8_t* data, uint16_t length)
	{
		if (length > OPPVS_NETWORK_PACKET_LENGTH - MESSAGE_HEADER_SIZE)
			length = OPPVS_NETWORK_PACKET_LENGTH - MESSAGE_HEADER_SIZE;
		memcpy(m_data + MESSAGE_HEADER_SIZE, data, length);
		//printHashCode(data, length);
		m_length = length + MESSAGE_HEADER_SIZE;
	}

	uint8_t* Message::getData()
	{
		return m_data;
	}

	uint16_t Message::getLength()
	{
		return m_length;
	}

	void Message::setLength(uint16_t len)
	{
		m_length = len;
	}

	void Message::setTimestamp(uint32_t ts)
	{
		m_timestamp = ts;
	}

	uint32_t Message::getTimestamp()
	{
		return m_timestamp;
	}

	void* runEncoding(void* p)
	{
		MessageHandling* msgHandler = (MessageHandling*)p;
		while (1)
		{
			msgHandler->encodeMessage();
			usleep(2000);
		}
		return NULL;
	}

	MessageHandling::MessageHandling() : m_numFramesInPool(0), m_sentClients(0)
	{
		m_timestamp = 0;
		m_encodingThread = new Thread(runEncoding, (void*)this);
		m_encodingThread->create();
	}

	MessageHandling::~MessageHandling()
	{
		delete m_encodingThread;
	}


	bool MessageHandling::isEmptyPool()
	{
		return m_messagePool.empty();
	}

	void MessageHandling::addMessage(PixelBuffer& pf)
	{
		if (m_numFramesInPool >= MAX_FRAMES_IN_POOL)
			return;

		std::shared_ptr<PixelBuffer> pixelbuffer(new PixelBuffer);
		pixelbuffer->source = pf.source;
		pixelbuffer->plane[0] = new uint8_t[pf.nbytes];
		memcpy(pixelbuffer->plane[0], pf.plane[0], pf.nbytes);
		pixelbuffer->nbytes = pf.nbytes;
		pixelbuffer->width[0] = pf.width[0];
		pixelbuffer->height[0] = pf.height[0];
		pixelbuffer->stride[0] = pf.stride[0];

		m_framePool.push(pixelbuffer);
		m_numFramesInPool++;

	}

	void MessageHandling::encodeMessage()
	{
		if (m_framePool.empty())
			return;

		std::shared_ptr<std::shared_ptr<PixelBuffer>> ptrFrame = m_framePool.pop();
		if (ptrFrame.get() == NULL)
		{
			return;
		}

		//m_numFramesInPool--;
		std::shared_ptr<PixelBuffer> frame = *ptrFrame;
		PixelBuffer pf = *frame;
		//Encoding
		uint8_t* data = NULL;
		uint32_t encodingLength = 0;
		bool isKey;
		int picID = -1;
		if (m_encoder->encode(pf, &encodingLength, &data, &picID, &isKey) < 0)
		{
			delete [] pf.plane[0];
			return;
		}
		delete [] pf.plane[0];

		//printf("Length: %u\n", encodingLength);

		int msgLength = encodingLength;
		int sendLength = msgLength > (OPPVS_NETWORK_PACKET_LENGTH - MESSAGE_HEADER_SIZE) ? (OPPVS_NETWORK_PACKET_LENGTH - MESSAGE_HEADER_SIZE) : msgLength;
		const uint8_t* curPos = data;
		uint16_t count = 0;
		while (msgLength > 0)
		{
			uint8_t req = XBit;
			if (count == 0)
				req |= SBit;
			uint8_t optX = IBit;
			uint8_t optY = (picID & 127);


			std::shared_ptr<Message> message(new Message);
			message->setSource(pf.source);
			message->setData(curPos, sendLength);
			message->setSegID(count);
			message->setTimestamp(m_timestamp);

			message->setVP8Required(req);
			message->setVP8OptX(optX);
			message->setVP8OptY(optY);

			curPos += sendLength;
			msgLength -= sendLength;

			if (count == 0)
			{
				if (msgLength > 0)
					message->setFlag(FLAG_START_FRAME);
				else
					message->setFlag(FLAG_ONE_FRAME);

				uint8_t o1 = (encodingLength & Size0BitMask) << Size0BitShift; // Size0
				o1 |= HBit; // H (show frame)
				if (!isKey)
				{
					o1 |= 1; //P (Inverse frame)
				}
				else
					printf("Key frame \n");

				message->setSize0(o1);
				message->setSize1(static_cast<uint8_t>(encodingLength >> 3));	//Size 1
				message->setSize2(static_cast<uint8_t>(encodingLength >> 11));	//Size 2

			}
			else if (msgLength <= 0)
			{
				message->setFlag(FLAG_END_FRAME);
			}
			else
			{
				message->setFlag(FLAG_MIDDLE_FRAME);
			}

			m_messagePool.push(message);
			count++;
			sendLength = msgLength > (OPPVS_NETWORK_PACKET_LENGTH - MESSAGE_HEADER_SIZE) ? (OPPVS_NETWORK_PACKET_LENGTH - MESSAGE_HEADER_SIZE) : msgLength;
		}

		//printf("Count: %d\n", count);
		m_timestamp++;


	}

	void MessageHandling::getNextMessage(uint8_t** pdata, uint16_t* length, uint32_t* ts)
	{
		if (m_messagePool.size() == 0)
		{
			return;
		}
		std::shared_ptr<std::shared_ptr<Message>> ptrmsg = m_messagePool.try_front();
		if (ptrmsg.get() != NULL)
		{
			std::shared_ptr<Message> message = *ptrmsg;
			*pdata = message->getData();
			*length = message->getLength();
			*ts = message->getTimestamp();
			m_sentClients++;
			currentMessage = ptrmsg;
		}
	}

	void MessageHandling::setNumClients(uint8_t clients)
	{
		m_numClients = clients;
	}

	bool MessageHandling::releaseMessage()
	{
		if (m_sentClients >= m_numClients)
		{
			//printf("Sent Client: %d \n", m_sentClients);
			std::shared_ptr<std::shared_ptr<Message>> ptrmsg = m_messagePool.try_front();
			if (ptrmsg.get() != NULL)
			{
				if (*ptrmsg == *currentMessage)
				{
					ptrmsg = m_messagePool.try_pop();
			
					if (ptrmsg.get() != NULL)
					{
						//printf("Pop\n");
						m_sentClients = 0;
						std::shared_ptr<Message> message = *ptrmsg;
						if (message->getFlag() == FLAG_END_FRAME || message->getFlag() == FLAG_ONE_FRAME)
							m_numFramesInPool--;
					}
				}
			}

			
			return true;
		}
		else
		{			
			return false;
		}
	}

	void MessageHandling::setEncoder(VPVideoEncoder* encoder)
	{	
		m_encoder = encoder;
	}

	MessageParsing::MessageParsing()
	{
		m_cacheBuffer = NULL;
		oldseq = -1;
		m_totalLength = -1;
		isSeenKeyFrame = false;
	}

	MessageParsing::~MessageParsing()
	{

	}

	void MessageParsing::setCacheBuffer(CacheBuffer *cb)
	{
		m_cacheBuffer = cb;
	}

	void MessageParsing::updateMessage(Message& message)
	{
		if (!m_cacheBuffer)
			return;
		uint32_t loc = 0;
		uint8_t req;
		uint8_t optx;
		uint8_t opty;
		int picID;
		uint8_t o1;
		bool showFrame = false;
		bool keyFrame = false;

		req = message.getVP8Required();
		if ((message.getFlag() == FLAG_START_FRAME) && !(req & SBit))
			return;

		if (req &XBit)
		{
			optx = message.getVP8OptX();
			if (optx & IBit)
			{
				opty = message.getVP8OptY();
				picID = opty >> 1;
			}
		}

		switch (message.getFlag())
		{
			case FLAG_START_FRAME:
			case FLAG_ONE_FRAME:
				m_cacheBuffer->delocateBuffer(message.getSource());
				m_cacheBuffer->allocateBuffer(message.getSource());
				m_currentTimestamp = message.getTimestamp();
				oldseq = message.getSegID();
				m_totalLength = 0;

				o1 = message.getSize0();
				showFrame = o1 & HBit;
				keyFrame = !(o1 & 1);
				o1 >>= Size0BitShift;
				//printf("Size: %u \n", o1 + 8 * message.getSize1() + 2048 * message.getSize2());
				if (keyFrame)
					printf("Key frame\n");
				if (keyFrame && !isSeenKeyFrame)
				{
					isSeenKeyFrame = true;
				}
				break;
			case FLAG_MIDDLE_FRAME:
				if (m_totalLength == -1)				
					//Lost first segement;
					return;
			
				/*if (m_currentTimestamp != message.getTimestamp())
					printf("Error\n");
				else
					if (oldseq != message.getSegID() - 1)
						printf("Lost segment\n");
					oldseq = message.getSegID();*/
				break;
		}

		if (!isSeenKeyFrame)
			return;

		loc = message.getSegID() * (OPPVS_NETWORK_PACKET_LENGTH - MESSAGE_HEADER_SIZE);
		uint8_t* dest = m_cacheBuffer->getBufferAddress(message.getSource(), loc);
		if (dest)
		{
			//printf("Loc %u %d %d\n", loc, message.getLength() - MESSAGE_HEADER_SIZE);
			memcpy(dest, message.getData() + MESSAGE_HEADER_SIZE, message.getLength() - MESSAGE_HEADER_SIZE);
			//printHashCode(message.getData() + MESSAGE_HEADER_SIZE, message.getLength() - MESSAGE_HEADER_SIZE);
			m_totalLength += message.getLength() - MESSAGE_HEADER_SIZE;
		}

		if (message.getFlag() == FLAG_ONE_FRAME)
		{
			m_cacheBuffer->push(message.getSource(), message.getLength() - MESSAGE_HEADER_SIZE);
			m_totalLength = -1;
		}
		else if (message.getFlag() == FLAG_END_FRAME)
		{
			if (oldseq != -1)
			{
				uint32_t len = (message.getSegID() - oldseq)*(OPPVS_NETWORK_PACKET_LENGTH - MESSAGE_HEADER_SIZE) + message.getLength() - MESSAGE_HEADER_SIZE;
				//printf("Correct len: %u %u\n", len, m_totalLength);
				if (len == m_totalLength)
					m_cacheBuffer->push(message.getSource(), m_totalLength);	
			}
			
			m_totalLength = -1;
			oldseq = -1;
		}
	}
}