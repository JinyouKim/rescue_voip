#include <opus/opus.h>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <memory.h>
#include "voice_encoder.h"

VoiceEncoder::VoiceEncoder(int _frameSize, int _sampleRate, int _channels, int _bitRate) {
	frameSize = _frameSize;
	sampleRate = _sampleRate;
	channels = _channels;
	bitRate = _bitRate;

	encoder = opus_encoder_create(sampleRate, channels, OPUS_APPLICATION_AUDIO, &error);

	if (error < 0) {
		fprintf(stderr, "opus_encoder_create: %s\n", opus_strerror(error));
		exit(-1);
	}
	
}

int VoiceEncoder::encode(const opus_int16* _pcm, unsigned char* _data, const size_t _bytesPerFrame) {
	error = opus_encode(encoder, _pcm, frameSize, _data, _bytesPerFrame);
	if (error < 0) {
		fprintf(stderr, "opus_encoder_create: %s\n", opus_strerror(error));
		return -1;
	}
	return error;
}

void VoiceEncoder::destroy() {
	opus_encoder_destroy(encoder);	
}

