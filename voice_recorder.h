#include <alsa/asoundlib.h>

#define PCM_DEVICE "default"

class VoiceRecorder {
	public:
		VoiceRecorder(unsigned int _samplingRate, int _voiceFrames);
		int getVoiceFrame(int16_t voiceFrame[]);
		void destroy();

	private:
		snd_pcm_hw_params_t *hw_params;
		unsigned int samplingRate;
		int voiceFrames;
		snd_pcm_t *captureHandle;
		int err;

};
