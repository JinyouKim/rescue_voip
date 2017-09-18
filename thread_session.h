#include "rtppacket.h"
#include "rtpsourcedata.h"
#include "rtpsession.h"
#include "voice_player.h"
#include "voice_decoder.h"
#include "stream_info.h"
#include <boost/interprocess/ipc/message_queue.hpp>
#include <unordered_map>
#include <iostream>
#include <string>

using namespace std;
using namespace jrtplib;
using namespace boost::interprocess;

class ThreadRTPSession : public RTPSession {
	public:
		ThreadRTPSession(RTPRandom *_rnd, uint16_t _destPort);
	protected:
		void OnPollThreadStart(bool& stop);
		void OnPollThreadStop();
		void OnPollThreadStep();
		void OnNewSource(RTPSourceData *_rtpSourceData);
		void OnRemoveSource(RTPSourceData *_rtpSourceData);
		void OnBYEPacket(RTPSourceData *_rtpSourceData);

		void ProcessRTPPacket(RTPSourceData *rtpSourceData, RTPPacket *rtpPacket);
	private: 
		bool isStop = false, isCalling;
		unordered_map<uint32_t, StreamInfo* > streamMap;
		VoiceDecoder *vd;
		void* pcmBuffer;
		uint16_t destPort;
		int test;
		message_queue *messageQueue;
		uint32_t callingSSRC;
		StreamInfo *currentStreamInfo;

	private:
		int addAddress(uint32_t destIP, uint16_t destPort);
		int deleteAddress(uint32_t destIP, uint16_t destPort);

};
