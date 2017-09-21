#include <alsa/asoundlib.h>
#include <mutex>
#include <boost/interprocess/ipc/message_queue.hpp>

#define PCM_DEVICE "default"
using namespace boost::interprocess;

class VoiceRecorder {
	public:
		VoiceRecorder(unsigned int _samplingRate, int _voiceFrames);
		void run();
		void stop();
		int getVoiceFrame(int16_t voiceFrame[]);
		void destroy();

	private:
		std::mutex mutexLock;
		message_queue *messageQueue;
		snd_pcm_hw_params_t *hw_params;
		unsigned int samplingRate;
		int voiceFrames;
		bool isRunning;
		snd_pcm_t *captureHandle;
		int err;

};
