#include "audio_engine.hpp"
#include "mac_audio_capture.hpp"

#import <AudioToolbox/AudioToolbox.h>
#import <CoreFoundation/CoreFoundation.h>

namespace oppvs {

	class MacAudioEngine : public AudioEngine
	{
	public:
		MacAudioEngine();
		~MacAudioEngine();
		
		void getListAudioDevices(std::vector<AudioDevice>& result);
		int addNewCapture(uint32_t deviceid);
		int init();
		int setInputDevice(AudioDeviceID deviceid, AudioComponentInstance& instance);
	private:
		
	};
} // oppvs