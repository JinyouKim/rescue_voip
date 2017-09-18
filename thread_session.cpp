#include <iostream>
#include "rtppacket.h"
#include "rtpipv4address.h"
#include "thread_session.h"
#include "rtpsession.h"
#include "rtpsourcedata.h"
#include "voice_decoder.h"
#include "voice_player.h"
#include <boost/interprocess/ipc/message_queue.hpp>

using namespace boost::interprocess;
using namespace std;
using namespace jrtplib;

ThreadRTPSession::ThreadRTPSession(RTPRandom *_rnd, uint16_t _destPort) {
	test = 0;
	vd = new VoiceDecoder(8000, 1);
	destPort = _destPort;
	isCalling = false;
	callingSSRC = -1;
}

void ThreadRTPSession::OnPollThreadStart(bool& stop) {
}

void ThreadRTPSession::OnPollThreadStop() {
	vd->destroy();
}

void ThreadRTPSession::OnPollThreadStep() {
	BeginDataAccess();

	if (GotoFirstSourceWithData()) {
		do {
			RTPPacket *rtpPacket;
			RTPSourceData *sourceData;
			uint32_t sourceIP;

			sourceData = GetCurrentSourceInfo();
			
			auto search = streamMap.find(sourceData->GetSSRC());
			if (search == streamMap.end()) {
				OnNewSource(sourceData);
			}
			currentStreamInfo = streamMap[sourceData->GetSSRC()];
			
			while ((rtpPacket = GetNextPacket()) != NULL) {
				ProcessRTPPacket(sourceData, rtpPacket);
				DeletePacket(rtpPacket);
			}
		} while (GotoNextSourceWithData());
	}

	EndDataAccess();
}

void ThreadRTPSession::ProcessRTPPacket(RTPSourceData *_rtpSourceData, RTPPacket *_rtpPacket) {
	if (isCalling && callingSSRC != _rtpSourceData->GetSSRC())
			return;

	if (!(_rtpPacket->GetPayloadType() == 20)) {
		int16_t *buffer = (int16_t *)malloc(currentStreamInfo->getBufferSize());
		size_t dataLength = _rtpPacket->GetPayloadLength();
		message_queue *messageQueue = currentStreamInfo->getMessageQueue();
		messageQueue->send(_rtpPacket->GetPayloadData(), dataLength, 0);

		free(buffer);
	}
	else {
		cout<<"bye"<<endl;
		isCalling = false;
		OnRemoveSource(_rtpSourceData);
		delete streamMap[_rtpSourceData->GetSSRC()];
		streamMap.erase(_rtpSourceData->GetSSRC());
		
	}
}

void ThreadRTPSession::OnNewSource(RTPSourceData *_rtpSourceData) {
	if (isCalling)
		return;
	else {
		if (_rtpSourceData->IsOwnSSRC())
			return;

		uint32_t ip;
		uint16_t port;

		if (_rtpSourceData->GetRTPDataAddress() != 0) {
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(_rtpSourceData->GetRTPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort();

		}
		else if (_rtpSourceData->GetRTCPDataAddress() != 0) {
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(_rtpSourceData->GetRTPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort()-1;
		}
		else 
			return;

		cout << ((ip >> 24) & 0xFF) << endl;

		if ((ip >> 24) & 0xFF == 192) {
			isCalling = true;
			callingSSRC = _rtpSourceData->GetSSRC();
			ClearDestinations();
		}
			
		cout << streamMap.size() << endl;
		streamMap[_rtpSourceData->GetSSRC()] = new StreamInfo(8000, 1, _rtpSourceData->GetSSRC());
		addAddress(ip, port);
	}

}

void ThreadRTPSession::OnBYEPacket(RTPSourceData *_rtpSourceData) {	
	cout << "byepacket"<<endl;
	if (_rtpSourceData->IsOwnSSRC())
			return;
		
	uint32_t ip;
	uint16_t port;

	if (_rtpSourceData->GetRTPDataAddress() != 0) {
		const RTPIPv4Address *addr = (const RTPIPv4Address *)(_rtpSourceData->GetRTPDataAddress());
		ip = addr->GetIP();
		port = addr->GetPort();
	}
	else if (_rtpSourceData->GetRTCPDataAddress() != 0) {
		const RTPIPv4Address *addr = (const RTPIPv4Address *)(_rtpSourceData->GetRTCPDataAddress());
		ip = addr->GetIP();
		port = addr->GetPort()-1;
	}
	else
		return;

	deleteAddress(ip, port);
}

void ThreadRTPSession::OnRemoveSource(RTPSourceData *_rtpSourceData) {	
	if (_rtpSourceData->IsOwnSSRC())
		return;
	if (_rtpSourceData->ReceivedBYE())
		return;
		
	uint32_t ip;
	uint16_t port;

	if (_rtpSourceData->GetRTPDataAddress() != 0) {
		const RTPIPv4Address *addr = (const RTPIPv4Address *)(_rtpSourceData->GetRTPDataAddress());
		ip = addr->GetIP();
		port = addr->GetPort();
	}
	else if (_rtpSourceData->GetRTCPDataAddress() != 0) {
		const RTPIPv4Address *addr = (const RTPIPv4Address *)(_rtpSourceData->GetRTCPDataAddress());
		ip = addr->GetIP();
		port = addr->GetPort()-1;
	}
	else
		return;

	deleteAddress(ip, port);
}

int ThreadRTPSession::addAddress(uint32_t destIP, uint16_t destPort) {
	RTPIPv4Address addr(destIP, destPort, true);
	return AddDestination(addr);
}

int ThreadRTPSession::deleteAddress(uint32_t destIP, uint16_t destPort) {
	RTPIPv4Address dest(destIP, destPort);
	return DeleteDestination(dest);
}
