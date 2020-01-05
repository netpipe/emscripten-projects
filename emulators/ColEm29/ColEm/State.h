/** ColEm: portable Coleco emulator **************************/
/**                                                         **/
/**                         State.h                         **/
/**                                                         **/
/** This file contains routines to save and load emulation  **/
/** state.                                                  **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2014                 **/
/**     The contents of this file are property of Marat     **/
/**     Fayzullin and should only be used as agreed with    **/
/**     him. The file is confidential. Absolutely no        **/
/**     distribution allowed.                               **/
/*************************************************************/
#ifndef STATE_H
#define STATE_H

#define SaveSTRUCT(Name) \
  if(Size+sizeof(Name)>MaxSize) return(0); \
  else { memcpy(Buf+Size,&(Name),sizeof(Name));Size+=sizeof(Name); }

#define SaveARRAY(Name) \
  if(Size+sizeof(Name)>MaxSize) return(0); \
  else { memcpy(Buf+Size,(Name),sizeof(Name));Size+=sizeof(Name); }

#define SaveDATA(Name,DataSize) \
  if(Size+(DataSize)>MaxSize) return(0); \
  else { memcpy(Buf+Size,(Name),(DataSize));Size+=(DataSize); }

#define LoadSTRUCT(Name) \
  if(Size+sizeof(Name)>MaxSize) return(0); \
  else { memcpy(&(Name),Buf+Size,sizeof(Name));Size+=sizeof(Name); }

#define SkipSTRUCT(Name) \
  if(Size+sizeof(Name)>MaxSize) return(0); \
  else Size+=sizeof(Name)

#define LoadARRAY(Name) \
  if(Size+sizeof(Name)>MaxSize) return(0); \
  else { memcpy((Name),Buf+Size,sizeof(Name));Size+=sizeof(Name); }

#define LoadDATA(Name,DataSize) \
  if(Size+(DataSize)>MaxSize) return(0); \
  else { memcpy((Name),Buf+Size,(DataSize));Size+=(DataSize); }

#define SkipDATA(DataSize) \
  if(Size+(DataSize)>MaxSize) return(0); \
  else Size+=(DataSize)

/** SaveState() **********************************************/
/** Save emulation state to a memory buffer. Returns size   **/
/** on success, 0 on failure.                               **/
/*************************************************************/
unsigned int SaveState(unsigned char *Buf,unsigned int MaxSize)
{
  unsigned int State[256],Size;
  byte *P;
  int J,I;

  /* No data written yet */
  Size = 0;

  /* Generate hardware state */
  J=0;
  memset(State,0,sizeof(State));
  State[J++] = Mode;
  State[J++] = UPeriod;
  State[J++] = ROMPage[0]-RAM;
  State[J++] = ROMPage[1]-RAM;
  State[J++] = ROMPage[2]-RAM;
  State[J++] = ROMPage[3]-RAM;
  State[J++] = ROMPage[4]-RAM;
  State[J++] = ROMPage[5]-RAM;
  State[J++] = ROMPage[6]-RAM;
  State[J++] = ROMPage[7]-RAM;
  State[J++] = RAMPage[0]-RAM;
  State[J++] = RAMPage[1]-RAM;
  State[J++] = RAMPage[2]-RAM;
  State[J++] = RAMPage[3]-RAM;
  State[J++] = RAMPage[4]-RAM;
  State[J++] = RAMPage[5]-RAM;
  State[J++] = RAMPage[6]-RAM;
  State[J++] = RAMPage[7]-RAM;
  State[J++] = JoyMode;
  State[J++] = Port20;
  State[J++] = Port60;

  /* Write out data structures */
  SaveSTRUCT(CPU);
  SaveSTRUCT(VDP);
  SaveSTRUCT(PSG);
  SaveARRAY(State);
  SaveDATA(RAM_BASE,0xA000);
  SaveDATA(VDP.VRAM,0x4000);

  /* Return amount of data written */
  return(Size);
}

/** LoadState() **********************************************/
/** Load emulation state from a memory buffer. Returns size **/
/** on success, 0 on failure.                               **/
/*************************************************************/
unsigned int LoadState(unsigned char *Buf,unsigned int MaxSize)
{
  int State[256],XPal[16],J;
  unsigned int Size;
  byte *VRAM;
  void *XBuf;

  /* No data read yet */
  Size = 0;

  /* Read CPU state */
  LoadSTRUCT(CPU);

  /* Read VDP state, preserving VRAM address */
  memcpy(XPal,VDP.XPal,sizeof(XPal));
  VRAM = VDP.VRAM;
  XBuf = VDP.XBuf;
  LoadSTRUCT(VDP);
  memcpy(VDP.XPal,XPal,sizeof(VDP.XPal));
  VDP.ChrTab += VRAM-VDP.VRAM;
  VDP.ChrGen += VRAM-VDP.VRAM;
  VDP.SprTab += VRAM-VDP.VRAM;
  VDP.SprGen += VRAM-VDP.VRAM;
  VDP.ColTab += VRAM-VDP.VRAM;
  VDP.VRAM    = VRAM;
  VDP.XBuf    = XBuf;

  /* Read in remaining data structures */
  LoadSTRUCT(PSG);
  LoadARRAY(State);
  LoadDATA(RAM_BASE,0xA000);
  LoadDATA(VDP.VRAM,0x4000);

  /* Parse hardware state */
  J=0;
  Mode       = State[J++];
  UPeriod    = State[J++];
  ROMPage[0] = State[J++]+RAM;
  ROMPage[1] = State[J++]+RAM;
  ROMPage[2] = State[J++]+RAM;
  ROMPage[3] = State[J++]+RAM;
  ROMPage[4] = State[J++]+RAM;
  ROMPage[5] = State[J++]+RAM;
  ROMPage[6] = State[J++]+RAM;
  ROMPage[7] = State[J++]+RAM;
  RAMPage[0] = State[J++]+RAM;
  RAMPage[1] = State[J++]+RAM;
  RAMPage[2] = State[J++]+RAM;
  RAMPage[3] = State[J++]+RAM;
  RAMPage[4] = State[J++]+RAM;
  RAMPage[5] = State[J++]+RAM;
  RAMPage[6] = State[J++]+RAM;
  RAMPage[7] = State[J++]+RAM;
  JoyMode    = State[J++];
  Port20     = State[J++];
  Port60     = State[J++];

  /* All PSG channels have been changed */
  PSG.Changed = 0xFF;

  /* Return amount of data read */
  return(Size);
}

/** SaveSTA() ************************************************/
/** Save emulation state into a .STA file. Returns 1 on     **/
/** success, 0 on failure.                                  **/
/*************************************************************/
int SaveSTA(const char *Name)
{
  static byte Header[16] = "STF\032\002\0\0\0\0\0\0\0\0\0\0\0";
  unsigned int Size,J;
  byte *Buf;
  FILE *F;

  /* Fail if no state file */
  if(!Name) return(0);

  /* Allocate temporary buffer */
  Buf = malloc(MAX_STASIZE);
  if(!Buf) return(0);

  /* Try saving state */
  Size = SaveState(Buf,MAX_STASIZE);
  if(!Size) { free(Buf);return(0); }

  /* Open new state file */
  F = fopen(Name,"wb");
  if(!F) { free(Buf);return(0); }

  /* Fill header */
  J = CartCRC();
  Header[5] = Mode&CV_ADAM;
  Header[6] = J&0xFF;
  Header[7] = (J>>8)&0xFF;
  Header[8] = (J>>16)&0xFF;
  Header[9] = (J>>24)&0xFF;

  /* Write out the header and the data */
  if(F && (fwrite(Header,1,16,F)!=16))  { fclose(F);F=0; }
  if(F && (fwrite(Buf,1,Size,F)!=Size)) { fclose(F);F=0; }

  /* If failed writing state, delete open file */
  if(F) fclose(F); else unlink(Name);

  /* Done */
  free(Buf);
  return(!!F);
}

/** LoadSTA() ************************************************/
/** Load emulation state from a .STA file. Returns 1 on     **/
/** success, 0 on failure.                                  **/
/*************************************************************/
int LoadSTA(const char *Name)
{
  byte Header[16],*Buf;
  int Size,OldMode,J;
  FILE *F;

  /* Fail if no state file */
  if(!Name) return(0);

  /* Open saved state file */
  if(!(F=fopen(Name,"rb"))) return(0);

  /* Read and check the header */
  if(fread(Header,1,16,F)!=16)       { fclose(F);return(0); }
  if(memcmp(Header,"STF\032\002",5)) { fclose(F);return(0); }
  J = CartCRC();
  if(
    (Header[5]!=(Mode&CV_ADAM)) ||
    (Header[6]!=(J&0xFF))       ||
    (Header[7]!=((J>>8)&0xFF))  ||
    (Header[8]!=((J>>16)&0xFF)) ||
    (Header[9]!=((J>>24)&0xFF))
  ) { fclose(F);return(0); }

  /* Allocate temporary buffer */
  Buf = malloc(MAX_STASIZE);
  if(!Buf) { fclose(F);return(0); }

  /* Read state into temporary buffer, then load it */
  OldMode = Mode;
  Size    = fread(Buf,1,MAX_STASIZE,F);
  Size    = Size>0? LoadState(Buf,Size):0;

  /* If failed loading state, reset hardware */
  if(!Size) ResetColeco(OldMode);

  /* Done */
  free(Buf);
  fclose(F);
  return(!!Size);
}

#endif /* STATE_H */
