//speech synth
//using espeak
//Written by Bill Heaster
/*
 * This is the most minimalistic approach to getting espeak running with the C api
 *
 *
 * compile with gcc -W -o myEspeak myEspeak.c -lespeak
 *
 * This was tested in ubuntu 15.10 I had to download the following packages.
 * espeak-data
 * libespeak-dev
 *
 * */

#include <stdio.h>
#include <string.h>
#include <speak_lib.h>




/* Must be called before any synthesis functions are called.
   This specifies a function in the calling program which is called when a buffer of
   speech sound data has been produced.

   The callback function is of the form:

int SynthCallback(short *wav, int numsamples, espeak_EVENT *events);

   wav:  is the speech sound data which has been produced.
      NULL indicates that the synthesis has been completed.

   numsamples: is the number of entries in wav.  This number may vary, may be less than
      the value implied by the buflength parameter given in espeak_Initialize, and may
      sometimes be zero (which does NOT indicate end of synthesis).

   events: an array of espeak_EVENT items which indicate word and sentence events, and
      also the occurance if <mark> and <audio> elements within the text.  The list of
      events is terminated by an event of type = 0.


   Callback returns: 0=continue synthesis,  1=abort synthesis.
*/
//don't delete this callback function.
int SynthCallback(short *wav, int numsamples, espeak_EVENT *events)
{

    return 0;
}


int main(void)
{
    espeak_ERROR speakErr;

    //must be called before any other functions
    //espeak initialize
    if(espeak_Initialize(AUDIO_OUTPUT_SYNCH_PLAYBACK,0,NULL,espeakINITIALIZE_PHONEME_EVENTS) <0)
    {
        puts("could not initialize espeak\n");
        return -1;
    }

    espeak_SetSynthCallback(SynthCallback);

    //make some text to spit out
    char textBuff[255]={0};

    strcpy(textBuff, "hello, hello, hello world");

    if((speakErr=espeak_Synth(textBuff, strlen(textBuff), 0,0,0,espeakCHARS_AUTO,NULL,NULL))!= EE_OK)
    {
        puts("error on synth creation\n");

    }



    return 0;
}
