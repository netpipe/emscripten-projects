/** ColEm: portable Coleco emulator **************************/
/**                                                         **/
/**                          ColEm.c                        **/
/**                                                         **/
/** This file contains generic main() procedure statrting   **/
/** the emulation.                                          **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2014                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/

#include "Coleco.h"
#include "EMULib.h"
#include "Help.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

char *Options[]=
{ 
  "verbose","pal","ntsc","skip","help",
  "adam","cv","allspr","autoa","noautoa","autob","noautob",
  "spin1x","spin1y","spin2x","spin2y",
  "drums","nodrums","logsnd","palette",
  "home","sound","nosound","trap",
  "sync","nosync","tv","notv","soft","nosoft",
  "saver","nosaver","shm","noshm","scale","vsync",
  0
};

extern const char *Title;/* Program title                     */
extern int   UseSound;   /* Sound mode                        */
extern int   UseZoom;    /* Scaling factor (UNIX)             */
extern int   UseEffects; /* EFF_* bits (UNIX/MSDOS)           */
extern int   SyncFreq;   /* Sync screen updates (UNIX/MSDOS)  */

/** main() ***************************************************/
/** This is a main() function used in Unix and MSDOS ports. **/
/** It parses command line arguments, sets emulation        **/
/** parameters, and passes control to the emulation itself. **/
/*************************************************************/

void main_loop(){
RunZ80(&CPU);
}
int main(int argc,char *argv[])
{
  char *CartName="./media/CART.ROM";
  char *P;
  int N,J,I;

#if defined(DEBUG)
  CPU.Trap=0xFFFF;
  CPU.Trace=0;
#endif

#if defined(MSDOS)
  Verbose=0;
#else
  Verbose=5;
#endif

  /* Set home directory to where executable is */
#if defined(MSDOS) || defined(WINDOWS)
  P=strrchr(argv[0],'\\');
#else
  P=strrchr(argv[0],'/');
#endif
  if(P) { *P='\0';HomeDir=argv[0]; }

  for(N=1,I=0;N<argc;N++)
    if(*argv[N]!='-')
      switch(I++)
      {
        case 0: CartName=argv[N];break;
        default: printf("%s: Excessive filename '%s'\n",argv[0],argv[N]);
      }
    else
    {    
      for(J=0;Options[J];J++)
        if(!strcmp(argv[N]+1,Options[J])) break;
      switch(J)
      {
        case 0:  N++;
                 if(N<argc) Verbose=atoi(argv[N]);
                 else printf("%s: No verbose level supplied\n",argv[0]);
                 break;
        case 1:  Mode|=CV_PAL;break;
        case 2:  Mode&=~CV_PAL;break;
  	case 3:  N++;
                 if(N>=argc)
                   printf("%s: No skipped frames percentage supplied\n",argv[0]);
                 else
                 {
                   J=atoi(argv[N]);
                   if((J>=0)&&(J<=99)) UPeriod=100-J; 
                 }
                 break;
	case 4:  printf
                 ("%s by Marat Fayzullin    (C)FMS 1994-2011\n",Title);
                 for(J=0;HelpText[J];J++) puts(HelpText[J]);
                 return(0);
        case 5:  Mode|=CV_ADAM;break;
        case 6:  Mode&=~CV_ADAM;break;
        case 7:  Mode|=CV_ALLSPRITE;break;
        case 8:  Mode|=CV_AUTOFIRER;break;
        case 9:  Mode&=~CV_AUTOFIRER;break;
        case 10: Mode|=CV_AUTOFIREL;break;
        case 11: Mode&=~CV_AUTOFIREL;break;
        case 12: Mode|=CV_SPINNER1X;break;
        case 13: Mode|=CV_SPINNER1Y;break;
        case 14: Mode|=CV_SPINNER2X;break;
        case 15: Mode|=CV_SPINNER2Y;break;
        case 16: Mode|=CV_DRUMS;break;
        case 17: Mode&=~CV_DRUMS;break;
        case 18: N++;
                 if(N<argc) SndName=argv[N];
                 else printf("%s: No file for soundtrack logging\n",argv[0]);
                 break;
        case 19: N++;
                 if(N>=argc)
                   printf("%s: No palette number supplied\n",argv[0]);
                 else
                 {
                   J    = atoi(argv[N]);
                   J    = J==1? CV_PALETTE1
                        : J==2? CV_PALETTE2
                        :       CV_PALETTE0;
                   Mode = (Mode&~CV_PALETTE)|J;
                 }
                 break;
        case 20: N++;
                 if(N<argc) HomeDir=argv[N];
                 else printf("%s: No home directory supplied\n",argv[0]);
                 break;
        case 21: N++;
                 if(N>=argc) { UseSound=1;N--; }
                 else if(sscanf(argv[N],"%d",&UseSound)!=1)
                      { UseSound=1;N--; }
                 break;
        case 22: UseSound=0;break;

#if defined(DEBUG)
        case 23: N++;
                 if(N>=argc)
                   printf("%s: No trap address supplied\n",argv[0]);
                 else
                   if(!strcmp(argv[N],"now")) CPU.Trace=1;
                   else sscanf(argv[N],"%hX",&CPU.Trap);
                 break;
#endif /* DEBUG */

#if defined(UNIX) || defined(MSDOS) || defined(MAEMO)
        case 24: N++;
                 if(N<argc) SyncFreq=atoi(argv[N]);
                 else printf("%s: No sync frequency supplied\n",argv[0]);
                 break;
        case 25: SyncFreq=0;break;
        case 26: UseEffects|=EFF_TVLINES;break;
        case 27: UseEffects&=~EFF_TVLINES;break;
        case 28: UseEffects|=EFF_SOFTEN;break;
        case 29: UseEffects&=~EFF_SOFTEN;break;
#endif /* UNIX || MSDOS || MAEMO */

#if defined(UNIX) || defined(MAEMO)
        case 30: UseEffects|=EFF_SAVECPU;break;
        case 31: UseEffects&=~EFF_SAVECPU;break;
#endif /* UNIX || MAEMO */

#if defined(UNIX)
#if defined(MITSHM)
        case 32: UseEffects|=EFF_MITSHM;break;
        case 33: UseEffects&=~EFF_MITSHM;break;
#endif
        case 34: N++;
                 if(N>=argc) { UseZoom=1;N--; }
                 else if(sscanf(argv[N],"%d",&UseZoom)!=1)
                      { UseZoom=1;N--; }
                 break;
#endif /* UNIX */

#if defined(MSDOS)
        case 35: SyncFreq=-1;break;
#endif /* MSDOS */

        default: printf("%s: Wrong option '%s'\n",argv[0],argv[N]);
      }
    }

  if(!InitMachine()) return(1);
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop,0,1);
#else
  StartColeco(CartName);
while(1){main_loop();}
#endif
  TrashColeco();
  TrashMachine();
  return(0);
}
