#include <cstdio>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include "voice_recorder.h"
#include "voice_decoder.h"
#include "voice_player.h"
#include "voice_encoder.h"
#include <iostream>

using namespace std;


VoiceRecorder::VoiceRecorder(unsigned int _samplingRate, int _voiceFrames) {
	samplingRate = _samplingRate;
	voiceFrames = _voiceFrames;
	
	if ((err= snd_pcm_open(&captureHandle, PCM_DEVICE, SND_PCM_STREAM_CAPTURE, 0)) > 0) {
		fprintf(stderr, "cannot open audio device %s (%s)\n", PCM_DEVICE, snd_strerror(err));
		exit(1);
	}
	
	fprintf(stdout, "audio interface opened\n");

	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
		fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	fprintf(stdout, "hw_params allocated\n");

	if ((err = snd_pcm_hw_params_any(captureHandle, hw_params)) < 0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	fprintf(stdout, "hw_params initialize\n");

	if ((err = snd_pcm_hw_params_set_access(captureHandle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf(stderr, "cannot set access type (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	fprintf(stdout, "hw_params access setted\n");

	if ((err = snd_pcm_hw_params_set_format(captureHandle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf(stderr, "cannot set sample format (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	fprintf(stdout, "hw_param_format setted\n");

	if ((err = snd_pcm_hw_params_set_rate(captureHandle, hw_params, samplingRate, 0)) < 0) {
   		fprintf (stderr, "cannot set sample rate (%s)\n",
        			snd_strerror (err));
	    	exit (1);
  	}
	
	fprintf(stdout, "hw_params rate setted\n");

	if ((err = snd_pcm_hw_params_set_channels (captureHandle, hw_params, 1)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n",
				snd_strerror (err));
		exit (1);
	}
	snd_pcm_uframes_t periodSize = 160, bufferSize = 160*1000;	
	err = snd_pcm_hw_params_set_period_size_near(captureHandle, hw_params, &periodSize, NULL);
	if (err < 0) {
		fprintf(stderr, "snd_pcm_hw_params_set_rate_near: %s\n", snd_strerror(err));
		exit(-1);
	}
	
	err = snd_pcm_hw_params_set_buffer_size_near(captureHandle, hw_params, &bufferSize);
	if (err < 0) {
		fprintf(stderr, "snd_pcm_hw_params_set_rate_near: %s\n", snd_strerror(err));
		exit(-1);
	}	

	fprintf(stdout, "hw_params channels setted\n");
	
	if ((err = snd_pcm_hw_params (captureHandle, hw_params)) < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params setted\n");
	

	fprintf(stdout, "hw_params freed\n");
	
	if ((err = snd_pcm_prepare (captureHandle)) < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
				snd_strerror(err));
		exit (1);
	}
	
	fprintf(stdout, "audio interface prepared\n");
	snd_pcm_prepare(captureHandle);
	snd_pcm_start(captureHandle);

}
		

int VoiceRecorder::getVoiceFrame(int16_t voiceFrame[]) {
	memset(voiceFrame, 0, sizeof(int16_t) * voiceFrames);
	
	long tmp;
		
	if ((err = snd_pcm_readi(captureHandle, voiceFrame,voiceFrames)) != voiceFrames) {
		cout <<err<<endl;
		fprintf (stderr, "read from audio interface failed (%s)\n", snd_strerror(err));
		snd_pcm_prepare(captureHandle);
		return -1;
	}
	return 0;
}

void VoiceRecorder::destroy() {
	snd_pcm_hw_params_free (hw_params);
	snd_pcm_close(captureHandle);
}

		

