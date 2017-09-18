#ifndef _VOICE_ENCODER_H_
#define _VOICE_ENCODER_H_

#include <opus/opus.h>

class VoiceEncoder {
	public:
		VoiceEncoder(int _frameSize = 160, int _sampleRate = 8000, int channels_ = 1, int _bitRate = 10000);
		int encode (const opus_int16 *_pcm, unsigned char* _data, const size_t _bytesPerFrame);
		void destroy();
	public:

	private:
		int frameSize, sampleRate, channels, bitRate;
		OpusEncoder *encoder;
		int error;
	

};

#endif
