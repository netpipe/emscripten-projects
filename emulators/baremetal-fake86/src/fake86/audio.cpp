/*
  Fake86: A portable, open-source 8086 PC emulator.
  Copyright (C)2010-2012 Mike Chambers

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/* audio.c: functions to mix the audio channels, and handle SDL's audio interface. */

#include "config.h"
#include "types.h"
#include "log.h"
#include "blaster.h"
#include "audio.h"

struct wav_hdr_s wav_hdr;

int8_t audbuf[96000];
int32_t audbufptr, usebuffersize, usesamplerate = AUDIO_DEFAULT_SAMPLE_RATE, latency = AUDIO_DEFAULT_LATENCY;
uint8_t speakerenabled = 0;

extern uint64_t gensamplerate, sampleticks, hostfreq;
extern int16_t adlibgensample();
extern int16_t speakergensample();
extern int16_t getssourcebyte();
extern int16_t getBlasterSample();

uint64_t doublesamplecount, cursampnum = 0, sampcount = 0, framecount = 0;
uint8_t bmpfilename[256];

int8_t samps[2400];

uint8_t audiobufferfilled() {
	if (audbufptr >= usebuffersize) return(1);
	return(0);
}

void tickaudio() {
	int16_t sample;
	if (audbufptr >= usebuffersize) return;
	sample = adlibgensample() >> 4;
	if (usessource) sample += getssourcebyte();
	sample += getBlasterSample();
	if (speakerenabled) sample += (speakergensample() >> 1);
	if (audbufptr < sizeof(audbuf) ) audbuf[audbufptr++] = (uint8_t) ((uint16_t) sample+128);
}

extern uint64_t timinginterval;
extern void inittiming();
void fill_audio (void *udata, int8_t *stream, int len) {
#ifdef TODO
	memcpy (stream, audbuf, len);
	memmove (audbuf, &audbuf[len], usebuffersize - len);
#endif

	/*sampcount += len;
	while (sampcount >= 2400) {
	sprintf(&bmpfilename[0], "j:\\bmp\\%08u.bmp", framecount++);
	_beginthread(savepic, 0, NULL);
	sampcount -= 2400;
	}*/

	audbufptr -= len;
	if (audbufptr < 0) audbufptr = 0;
}

void initaudio() {
	log ("Initializing audio stream... ");
#ifdef TODO
	if (usesamplerate < 4000) usesamplerate = 4000;
	else if (usesamplerate > 96000) usesamplerate = 96000;
	if (latency < 10) latency = 10;
	else if (latency > 1000) latency = 1000;
	audbufptr = usebuffersize = (usesamplerate / 1000) * latency;
	gensamplerate = usesamplerate;
	doublesamplecount = (uint32_t) ( (double) usesamplerate * (double) 0.01);

	wanted.freq = usesamplerate;
	wanted.format = AUDIO_U8;
	wanted.channels = 1;
	wanted.samples = (uint16_t) usebuffersize >> 1;
	wanted.callback = (void *) fill_audio;
	wanted.userdata = nullptr;

	if (SDL_OpenAudio (&wanted, nullptr) <0) {
			printf ("Error: %s\n", SDL_GetError() );
			return;
		}
	else {
			printf ("OK! (%lu Hz, %lu ms, %lu sample latency)\n", usesamplerate, latency, usebuffersize);
		}

	memset (audbuf, 128, sizeof (audbuf) );
	audbufptr = usebuffersize;
	//create_output_wav("fake86.raw");
	SDL_PauseAudio (0);
	return;
#endif
}

void killaudio() {
#if 0
	SDL_PauseAudio (1);

	if (wav_file == nullptr) return;
	//wav_hdr.ChunkSize = wav_hdr.Subchunk2Size + sizeof(wav_hdr) - 8;
	//fseek(wav_file, 0, SEEK_SET);
	//fwrite((void *)&wav_hdr, 1, sizeof(wav_hdr), wav_file);
	fclose (wav_file);
#endif
}
