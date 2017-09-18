#ifndef _STREAM_INFO_H_
#define _STREAM_INFO_H_
#include "voice_player.h"
#include <mutex>
#include<iostream>
#include <boost/interprocess/ipc/message_queue.hpp>

using namespace std;
using namespace boost::interprocess;

class VoicePlayer;

class StreamInfo {
	public:
		StreamInfo(unsigned int _samplingRate, unsigned int _channels, uint32_t _ssrc);
		~StreamInfo();
		void lock();
		void unlock();
		void lock2();
		void unlock2();
		//void setBuffer(void *_copyBuffer, size_t _dataLength);
		size_t getBufferSize();
		string getSSRCString();
		message_queue* getMessageQueue();
		

	private:
		std::mutex mutexLock, mutexLock2;
		size_t bufferSize;
		//void *data;
		VoicePlayer* player;
		//unsigned int bufferIndex;
		uint32_t ssrc;
		string ssrcString;
		message_queue *messageQueue;
};

#endif
