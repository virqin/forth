#include "signaling_message_builder.hpp"

namespace oppvs {
	SignalingMessageBuilder::SignalingMessageBuilder() 
	{

	}

	SignalingMessageBuilder::~SignalingMessageBuilder()
	{

	}

	DataStream& SignalingMessageBuilder::getDataStream()
	{
		return m_dataStream;
	}

	int SignalingMessageBuilder::getResult(SharedDynamicBufferRef& buffer)
	{
		int ret = addMessageLength();
		if (ret == 0)
			buffer = m_dataStream.getBuffer();
		return ret;
	}

	int SignalingMessageBuilder::addMessageType(SignalingMessageType type)
	{
		uint16_t msgType = type;
		m_dataStream.grow(100);
		if (m_dataStream.writeUInt16(htons(msgType)) < 0)
			return -1;

		//Reserve space for message length
		if (m_dataStream.writeUInt16(0) < 0)
	    	return -1;
		return 0;
	}

	int SignalingMessageBuilder::addStreamKey(uint32_t streamKey)
	{
		if (m_dataStream.writeUInt32(htonl(streamKey)) < 0)
			return -1;

		return 0;
	}

	int SignalingMessageBuilder::addMessageLength()
	{
		uint16_t len = m_dataStream.size();
    	size_t currentPos = m_dataStream.getPosition();
    	if (len < SIGNALING_HEADER_SIZE)
    	{
    		printf("Wrong size in signaling message\n");
    		return -1;
    	}

    	len -= SIGNALING_HEADER_SIZE;
    	m_dataStream.setAbsolutePosition(2);	//Message length is at 3 & 4th byte
    	m_dataStream.writeUInt16(htons(len));
    	m_dataStream.setAbsolutePosition(currentPos);
		return 0;
	}

	int SignalingMessageBuilder::addAttributeHeader(uint16_t type, uint16_t size)
	{
		int ret = 0;
    	ret = m_dataStream.writeUInt16(htons(type));
    	if (ret >= 0)
    		ret = m_dataStream.writeUInt16(htons(size));
    	return ret;
	}

	int SignalingMessageBuilder::addAttribute(uint16_t type, const void* data, uint16_t size)
    {
        uint8_t padBytes[4] = {0};
        size_t padding = 0;

        uint16_t sizeheader = size;

        if (data == NULL)
        {
            size = 0;
        }

        // attributes always start on a 4-byte boundary
        padding = (size % 4) ? (4 - (size % 4)) : 0;

        //zero length attributes as an indicator of something
        if (addAttributeHeader(type, sizeheader) < 0)
        	return -1;

        if (size > 0)
        {
        	if (m_dataStream.write(data, size) < 0)
            	return -1;
        }

        // pad with zeros to get the 4-byte alignment
        if (padding > 0)
        {
        	if (m_dataStream.write(padBytes, padding) < 0)
            	return -1;
        }
        return 0;
    }

	int SignalingMessageBuilder::addStringAttribute(uint16_t type, std::string s)
	{
		if (s == "")
			return -1;

		return addAttribute(type, s.c_str(), s.length());
	}

	int SignalingMessageBuilder::addIceUsername(std::string username)
	{
		return addStringAttribute(SIGNALING_ATTRIBUTE_ICE_USERNAME, username);
	}

	int SignalingMessageBuilder::addIcePassword(std::string password)
	{
		return addStringAttribute(SIGNALING_ATTRIBUTE_ICE_PASSWORD, password);
	}

	int SignalingMessageBuilder::addIceCandidates(std::vector<IceCandidate>& candidates)
	{
		uint16_t noCandidates = candidates.size();
		if (noCandidates == 0)
			return -1;

		noCandidates = htons(noCandidates);
		if (addAttribute(SIGNALING_ATTRIBUTE_ICE_NO_CANDIDATES, &noCandidates, 2) < 0)
			return -1;


		return 0;
	}
} // oppvs