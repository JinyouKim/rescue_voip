#ifndef _VOICE_DECODER_H_
#define _VOICE_DECODER_H_
#include <iostream>
#include <opus/opus.h>

class VoiceDecoder {
	public: 
		VoiceDecoder(int _sampleRate = 8000, int _channels = 1);
		int decode(const unsigned char* _data, opus_int32 _len, opus_int16* _pcm);
		void destroy();
		void* createPCMBuffer();
	private:
		int maxFrameSize, sampleRate, channels;
		OpusDecoder *decoder;
		int err;
};



#endif
