#ifndef _VOICE_PLAYER_H_
#define _VOICE_PLAYER_H_
#include <alsa/asoundlib.h>
#include <iostream>
#include <sys/poll.h>
#include <mutex>
#include "stream_info.h"
#include "voice_decoder.h"
#include <boost/interprocess/ipc/message_queue.hpp>
using namespace boost::interprocess;

#define PCM_DEVICE "default"

class StreamInfo;

class VoicePlayer {
	public:
		VoicePlayer(unsigned int _samplingRate, unsigned int _channels, StreamInfo *streamInfo);
		~VoicePlayer();
		void destroy();
		void run();
		void stop();
	private:
		void createHandle();
	private:
		VoiceDecoder *decoder;
		StreamInfo *streamInfo;
		std::mutex mutexLock;
		void *data;
		void *pcmBuffer;
		int count;
		snd_pcm_t *playerHandle;
		snd_pcm_hw_params_t *hwParams;
		snd_pcm_sw_params_t *swParams;
		snd_pcm_uframes_t periodSize;
		snd_pcm_uframes_t bufferSize;
		struct pollfd fds[16];
		unsigned int samplingRate, channels;
		bool isRunning;
		unsigned int err;
		unsigned int *zeroBuffer;
		message_queue *messageQueue;
};


#endif
