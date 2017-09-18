#include <alsa/asoundlib.h>
#include <cstdio>
#include "voice_player.h"
#include "stream_info.h"
#include <boost/interprocess/ipc/message_queue.hpp>
#include <iostream>
#include <mutex>
#include <memory.h>
#include <sys/time.h>

using namespace std;
using namespace boost::interprocess;

VoicePlayer::VoicePlayer(unsigned int _samplingRate, unsigned int _channels, StreamInfo *_streamInfo) {
	samplingRate = _samplingRate;
	channels = _channels;
	streamInfo = _streamInfo;
	messageQueue = new message_queue(open_only, (streamInfo->getSSRCString()).c_str());
	decoder = new VoiceDecoder(samplingRate, channels);
}

VoicePlayer::~VoicePlayer() {
}

void VoicePlayer::createHandle() {
	snd_pcm_hw_params_malloc(&hwParams);
	snd_pcm_sw_params_malloc(&swParams);

	snd_pcm_open(&playerHandle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
	snd_pcm_hw_params_any(playerHandle, hwParams);
	snd_pcm_hw_params_set_access(playerHandle, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(playerHandle, hwParams, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels_near(playerHandle, hwParams, &channels);
	snd_pcm_hw_params_set_rate_near(playerHandle, hwParams, &samplingRate, NULL);

	unsigned int outputSize = 160 * samplingRate / samplingRate;
	
	periodSize = outputSize;
	bufferSize = outputSize * 10;

	int dir = 1;
	snd_pcm_hw_params_set_period_size_near(playerHandle, hwParams, &periodSize, 0);
	snd_pcm_hw_params_set_buffer_size_near(playerHandle, hwParams, &bufferSize);
	snd_pcm_hw_params(playerHandle, hwParams);

	snd_pcm_sw_params_current(playerHandle,swParams);
	snd_pcm_sw_params_set_avail_min(playerHandle, swParams, periodSize);
	snd_pcm_sw_params_set_start_threshold(playerHandle, swParams, bufferSize - periodSize);
	snd_pcm_sw_params_set_stop_threshold(playerHandle, swParams, bufferSize);
	snd_pcm_sw_params(playerHandle, swParams);

	snd_pcm_prepare(playerHandle);

	snd_pcm_hw_params_free(hwParams);
	snd_pcm_sw_params_free(swParams);

	zeroBuffer = (unsigned int *) malloc (sizeof (unsigned int) * periodSize*channels);
	memset(zeroBuffer, 0, periodSize*channels*sizeof(unsigned int));	
	for (int i = 0; i < bufferSize / periodSize; i++) 
		snd_pcm_writei(playerHandle, zeroBuffer, periodSize);
}

void VoicePlayer::stop() {
	mutexLock.lock();
	isRunning = false;
	mutexLock.unlock();
}

void VoicePlayer::run() {
	isRunning = true;
	data = (void *)malloc(streamInfo->getBufferSize());
	pcmBuffer = (void *)malloc(streamInfo->getBufferSize());
	createHandle();
	mutexLock.lock();
	while (isRunning) {
		snd_pcm_sframes_t avail;
		avail = snd_pcm_avail_update(playerHandle);
		
		while (avail >= (int)periodSize) {
			unsigned int priority;
			message_queue::size_type messageSize;
			messageQueue->receive(data, streamInfo->getBufferSize(), messageSize, priority);
			err = 0;
			
			size_t length = decoder->decode((unsigned char *)data, messageSize, (int16_t*)pcmBuffer);
			err = snd_pcm_writei(playerHandle, pcmBuffer, length);
			struct timespec ts;
			clock_gettime(CLOCK_REALTIME, &ts);
			printf("receive time: %llu nsec", (ts.tv_sec * 1000000000L) + ts.tv_nsec);
			
			if (err < 0) {
				avail = err;
				break;
			}
			avail = snd_pcm_avail_update(playerHandle);
		}
		if (avail == -EPIPE) {
			cout <<"XRUN"<<endl;
			snd_pcm_drain(playerHandle);
			snd_pcm_prepare(playerHandle);
			for (int i = 0; i < bufferSize / periodSize; i++) 
				snd_pcm_writei(playerHandle, zeroBuffer, periodSize);
		}
		
	}
	mutexLock.unlock();

	destroy();
}

void VoicePlayer::destroy() {
	snd_pcm_drain(playerHandle);
	snd_pcm_close(playerHandle);
}

