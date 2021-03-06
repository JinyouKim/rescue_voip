#include "voice_server.h"
#include "rtpsession.h"
#include "rtptimeutilities.h"
#include "rtpsourcedata.h"
#include "rtpconfig.h"
#include "rtppacket.h"
#include "rtprandom.h"
#include "voice_recorder.h"
#include "voice_encoder.h"
#include <opus/opus.h>
#include <string>
#include <cstdio>
#include <stdlib.h>
#include <string>
#include <iostream>

using namespace std;

VoiceServer::VoiceServer(uint16_t _portbase, uint16_t _destPort) {
	portbase = _portbase;
	destPort = _destPort;
	RTPRandom *rnd= RTPRandom::CreateDefaultRandomNumberGenerator();
	session = new ThreadRTPSession(rnd, destPort);
}
void VoiceServer::setParameters(int _frameSize, int _sampleRate, int _channels, int _bitRate) {
	frameSize = _frameSize;
	sampleRate = _sampleRate;
	channels = _channels;
	bitRate = _bitRate;
	bytesPerFrame = bitRate * 1024 * frameSize / sampleRate / 8;
	tsPerFrame = frameSize * 8000 / sampleRate;


}

int VoiceServer::createSession(double _second, double _samples, int _payloadType, bool _isRTCPMultiplexing) {	
	// send 10 samples per sec
	sessionParams.SetOwnTimestampUnit(_second/_samples);
	sessionParams.SetAcceptOwnPackets(false);

	transmissionParams.SetPortbase(portbase);
	transmissionParams.SetRTCPMultiplexing(_isRTCPMultiplexing);

	status = session->Create(sessionParams, &transmissionParams);
	
	cout<<RTPGetErrorString(status)<<endl;

	session->SetDefaultPayloadType(_payloadType);
	session->SetDefaultMark(false);
	session->SetDefaultTimestampIncrement(tsPerFrame);


	cout<<RTPGetErrorString(status)<<endl;
	return status;
}

uint32_t VoiceServer::getAddressbyString(string _ip) {
	uint32_t ip = inet_addr(_ip.c_str());
	if (ip == INADDR_NONE) {
		cerr << "IP error" <<endl;
	    return -1;
	}
	ip = ntohl(ip);

	return ip;
}

int VoiceServer::addAddress(string _destIP) {
	uint32_t destIP = inet_addr(_destIP.c_str());
	if (destIP == INADDR_NONE) {
		cerr << "IP error" <<endl;
		return -1;
	}
	destIP = ntohl(destIP);
	RTPIPv4Address addr(destIP, destPort, true);
	status = session->AddDestination(addr);

	return status;
}

void VoiceServer::stopSendVoice() {
	mutexLock.lock();
	done = true;
	mutexLock.unlock();
}

int VoiceServer::startSendVoice () {
	int packetLength;

	VoiceRecorder vr(sampleRate, frameSize);
	VoiceEncoder ve(frameSize, sampleRate, channels, bitRate);

	
	int16_t *voiceFrame = (int16_t *)malloc(sizeof(int16_t) * frameSize * channels);
	unsigned char *bitStream = (unsigned char*)malloc(bytesPerFrame);
	done = false;
	while (true) {
		mutexLock.lock();
		if (done) {
			break;
		}
		mutexLock.unlock();
		int err = vr.getVoiceFrame(voiceFrame);
		if (err < 0)
			continue;
		else {
			packetLength = ve.encode(voiceFrame, bitStream, bytesPerFrame);
			status = session->SendPacket(bitStream, packetLength);
			if (status < 0) {
				cout<<RTPGetErrorString(status)<<endl;
				return status;
			}
		}
	}

	vr.destroy();
	ve.destroy();

	free(voiceFrame);
	free(bitStream);
	return status;
}
