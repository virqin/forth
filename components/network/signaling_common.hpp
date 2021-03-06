#ifndef OPPVS_SIGNALING_COMMON_HPP
#define OPPVS_SIGNALING_COMMON_HPP

#include <functional>
#include "datatypes.hpp"
#include "ice_common.hpp"

namespace oppvs {
	const static int DEFAULT_SIGNALING_PORT = 33333;
	const static int MAX_SIGNALING_MESSAGE_SIZE = 2000;
	const static int STREAM_KEY_SIZE = 36;
	const static int SIGNALING_HEADER_SIZE = STREAM_KEY_SIZE + 4;

	enum SignalingUserRole
	{
		SignalingBroadcaster = 0x01,
		SignalingViewer = 0x02
	};

	enum SignalingMessageType
	{
		SignalingStreamRegister = 0x01,
		SignalingIceRequest = 0x02,
		SignalingIceResponse = 0x03,
		SignalingStreamRequest = 0x04,
		SignalingStreamResponse = 0x05,
		SignalingPeerRegister = 0x06,
		SignalingInvalid = 0x00
	};

	const static uint16_t SIGNALING_ATTRIBUTE_ICE_USERNAME = 0x0001;
	const static uint16_t SIGNALING_ATTRIBUTE_ICE_PASSWORD = 0x0002;
	const static uint16_t SIGNALING_ATTRIBUTE_ICE_NO_CANDIDATES = 0x0003;
	const static uint16_t SIGNALING_ATTRIBUTE_ICE_FOUNDATION = 0x0004;
	const static uint16_t SIGNALING_ATTRIBUTE_ICE_PRIORITY = 0x0005;
	const static uint16_t SIGNALING_ATTRIBUTE_ICE_IP_ADDRESS = 0x0006;
	const static uint16_t SIGNALING_ATTRIBUTE_ICE_PORT = 0x0007;
	const static uint16_t SIGNALING_ATTRIBUTE_ICE_TYPE = 0x0008;

	const static uint16_t SIGNALING_ATTRIBUTE_VIDEO_NOSOURCES = 0x0009;
	const static uint16_t SIGNALING_ATTRIBUTE_SOURCE_VIDEO_WIDTH = 0x000A;
	const static uint16_t SIGNALING_ATTRIBUTE_SOURCE_VIDEO_HEIGHT = 0x000B;
	const static uint16_t SIGNALING_ATTRIBUTE_SOURCE_VIDEO_STRIDE = 0x000C;
	const static uint16_t SIGNALING_ATTRIBUTE_SOURCE_VIDEO_ID = 0x000D;
	const static uint16_t SIGNALING_ATTRIBUTE_SOURCE_VIDEO_ORDER = 0x000E;

	const static uint16_t SIGNALING_ATTRIBUTE_AUDIO_NOSOURCES = 0x000F;
	const static uint16_t SIGNALING_ATTRIBUTE_SOURCE_AUDIO_ID = 0x0010;
	const static uint16_t SIGNALING_ATTRIBUTE_SOURCE_AUDIO_CHANNELS = 0x0011;
	const static uint16_t SIGNALING_ATTRIBUTE_SOURCE_AUDIO_SAMPLE_RATE = 0x0012;

	const static uint16_t SIGNALING_ATTRIBUTE_DURATION_START = 0x0013;
	const static uint16_t SIGNALING_ATTRIBUTE_DURATION_END = 0x0014;

	const static uint16_t SIGNALING_ATTRIBUTE_SOURCE_VIDEO_PIXEL_FORMAT = 0x0015;

	inline SignalingMessageType convertToSignalingMessageType(uint16_t value)
	{
		switch (value)
		{
			case 1:
				return SignalingStreamRegister;
			case 2:
				return SignalingIceRequest;
			case 3:
				return SignalingIceResponse;
			case 4:
				return SignalingStreamRequest;
			case 5:
				return SignalingStreamResponse;
			case 6:
				return SignalingPeerRegister;
			default:
				return SignalingInvalid;
		}
	}

	//Callback functions types
	typedef std::function<int(const std::string&, int, const ServiceInfo&)> callbackStreamRegister;
	typedef std::function<int(const std::string&, int*, ServiceInfo&)> callbackStreamRequest;
	typedef std::function<int(const ServiceInfo&)> callbackStreamResponse;
	typedef std::function<int(int sockfd)> callbackDisconnect;
	typedef std::function<int(const std::string&, int, const AvailableDuration&)> callbackPeerRegister;
} // oppvs

#endif // OPPVS_SIGNALING_COMMON_HPP
