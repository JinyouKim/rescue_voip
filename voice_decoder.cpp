#include <opus/opus.h>
#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include "voice_decoder.h"
#include "stream_info.h"
#include <memory.h>
#include <mutex>

VoiceDecoder::VoiceDecoder(int _sampleRate, int _channels) {
	sampleRate = _sampleRate;
	channels = _channels;
	maxFrameSize = sampleRate * 0.001 * 120 * 2;
	
	decoder = opus_decoder_create(sampleRate, channels, &err);
	if (err < 0) {
		fprintf(stderr, "opus_decoder_create : %s\n", opus_strerror(err));
		exit(-1);
	}
}

int VoiceDecoder::decode(const unsigned char* _data, opus_int32 _len, opus_int16 *_buffer) {
	int frameSize = opus_decode(decoder, _data, _len, _buffer, maxFrameSize, 0);
	if (frameSize < 0) {
		fprintf(stderr, "opus_decode : %s\n", opus_strerror(frameSize));
		return -1;
	}
	return frameSize;
}

void VoiceDecoder::destroy() {
	opus_decoder_destroy(decoder);
}
