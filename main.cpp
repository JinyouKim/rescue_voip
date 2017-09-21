#include "voice_server.h"
#include "rtpconfig.h"
#include <iostream>
#include <cstdio>

using namespace std;

int main() {
	VoiceServer vc(8000, 8000);
	vc.setParameters(160, 8000, 1, 12);
	vc.createSession();
	//vc.addAddress("192.168.123.3",DEST_PORT);
	vc.joinMulticastGroup("239.0.0.1");
	vc.addAddress("192.168.123.4");
	//vc.addAddress("192.168.123.4");
	//vc.addAddress("192.168.123.5",DEST_PORT);
	//vc.addAddress("192.168.123.6",DEST_PORT);

	//while(1);
	vc.startSendVoice();

	return 0;
}
			
