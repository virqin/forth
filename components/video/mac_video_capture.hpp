/*

*/

#pragma once

#include "../interface/video_capture.hpp"

namespace oppvs {

	class MacVideoCapture : public VideoCapture {
		public:
			MacVideoCapture(frame_callback fc, void* user, const VideoActiveSource& source);
			~MacVideoCapture();

			void setup();
			void start();
			void stop();
	};

	class MacVideoEngine : public VideoEngine {
	public:
		MacVideoEngine();
		MacVideoEngine(frame_callback fc, void* user);
		~MacVideoEngine();

		void getListCaptureDevices(std::vector<VideoCaptureDevice>& result);

		void getListVideoSource(std::vector<VideoScreenSource>& result);

		void setupCaptureSessions();

		void startRecording();

		void stopRecording();

		int getDeviceID(std::string& title);
	};
}