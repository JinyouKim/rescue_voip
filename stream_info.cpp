#include "stream_info.h"
#include <memory.h>
#include <mutex>
#include <thread>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <string>

using namespace boost::interprocess;
using namespace std;

StreamInfo::StreamInfo(unsigned int _samplingRate, unsigned int _channels,  uint32_t _ssrc) {
	// buffer size per 20ms 
	bufferSize = _samplingRate * 120 * 0.01 * 2;
	ssrc = _ssrc;
	ssrcString = to_string(ssrc);

	message_queue::remove(ssrcString.c_str());
	messageQueue = new message_queue(create_only, ssrcString.c_str(), 100, bufferSize);
	player = new VoicePlayer(_samplingRate , _channels, this);
	boost::thread playerThread = boost::thread(boost::bind(&VoicePlayer::run, player));
	
}

StreamInfo::~StreamInfo() {
	player->stop();

	message_queue::remove(ssrcString.c_str());
}

void StreamInfo::lock() {
	mutexLock.lock();
}

void StreamInfo::lock2() {
	mutexLock2.lock();
}

void StreamInfo::unlock() {
	mutexLock.unlock();
}

void StreamInfo::unlock2() {
	mutexLock2.unlock();
}

/*
void StreamInfo::setBuffer(void* _copyBuffer, size_t _length) {
	size_t dataLength = _length;
	void* data = (void *)malloc(bufferSize);
	memcpy(data, _copyBuffer, dataLength * 2);
	messageQueue->send(data, dataLength * 2, 0);
	cout << "send" <<endl;
	free(data);
}
*/

size_t StreamInfo::getBufferSize() {
	return bufferSize;
}

string StreamInfo::getSSRCString() {
	return ssrcString;
}

message_queue* StreamInfo::getMessageQueue() {
	return messageQueue;
}

