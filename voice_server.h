#ifndef _VOICE_SERVER_H_
#define _VOICE_SERVER_H_

#define PORT_BASE 8000
#define DEST_PORT 8000 

#include <string>
#include <mutex>
#include "rtpsession.h"
#include "rtpudpv4transmitter.h"
#include "rtpsessionparams.h"
#include "thread_session.h"

using namespace jrtplib;
using namespace std;

class VoiceServer {
	public:
		VoiceServer(uint16_t _portbase = PORT_BASE, uint16_t _destport = DEST_PORT);
		int createSession(double _second = 1.0, double _samples = 8000.0, int _payloadType=18, bool _isRTCPMultiplexing=true);
		int addAddress(string _destIP);
		int startSendVoice();
		void stopSendVoice();
		void setParameters(int _frameSize, int _sampleRate, int _channels, int _bitRate);
		int joinMulticastGroup(string _groupIP);

	private:
		uint32_t getAddressbyString(string _ip);

	private:
		size_t bytesPerFrame;
		unsigned int tsPerFrame;
		uint16_t portbase, destPort;
		ThreadRTPSession *session;
		RTPSessionParams sessionParams;
		RTPUDPv4TransmissionParams transmissionParams;
		int status, i;
		int frameSize, sampleRate, channels, bitRate;
		bool done;
		std::mutex mutexLock;

};

#endif
