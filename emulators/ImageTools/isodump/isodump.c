/*
  isodump.c  --  extract iso9660 image from device or file
  Copyright (C) 1999-2002 Steffen Solyga <solyga@absinth.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* $Id: isodump.c,v 1.20 2008-01-30 22:27:09 solyga Exp $ */

#include	"isodump.h"


int
display_help( char* fn ) {
  fprintf( HELP_CHANNEL, "%s v%s (%s): ", fn, VERSION_NUMBER, DATE_OF_LAST_MOD );
  fprintf( HELP_CHANNEL, "Extract iso9660 image from device or file.\n" );
  fprintf( HELP_CHANNEL, "Flowers & bug reports to %s.\n", MY_EMAIL_ADDRESS );
  fprintf( HELP_CHANNEL, "Usage: %s [options] [infile [outfile]]\n", fn );
  fprintf( HELP_CHANNEL, "switches:\n" );
  fprintf( HELP_CHANNEL, "  -a\t extract all data available (until EOF)\n" );
  fprintf( HELP_CHANNEL, "  -h\t write this info to %s and exit sucessfully\n", HELP_CHANNEL==stdout ? "stdout" : "stderr" );
  fprintf( HELP_CHANNEL, "  -n\t NRG image (skip first 150 blocks)\n" );
  fprintf( HELP_CHANNEL, "  -p\t don't pad image (if it's too small)\n" );
  fprintf( HELP_CHANNEL, "  -v\t increase verbosity level on %s\n", VERBOSE_CHANNEL==stdout ? "stdout" : "stderr" );
  fprintf( HELP_CHANNEL, "  -x\t XA image (set block offset to %d)\n", CD_XA_SYNC_HEAD );
  fprintf( HELP_CHANNEL, "  -H\t print %d hash marks on %s\n", HASH_MARKS, VERBOSE_CHANNEL==stdout ? "stdout" : "stderr" );
  fprintf( HELP_CHANNEL, "  -V\t write version and compilation info to %s and exit sucessfully\n", VERSION_CHANNEL==stdout ? "stdout" : "stderr" );
  return( 0 );
}


int
display_version( char* pn ) {
  fprintf( VERSION_CHANNEL, "%s v%s (%s)\n", pn, VERSION_NUMBER, DATE_OF_LAST_MOD );
  fprintf( VERSION_CHANNEL, "compilation settings:\n" );
  fprintf( VERSION_CHANNEL, "  DEFAULT_INPUT_FILE : %s\n", DEFAULT_INPUT_FILE );
  fprintf( VERSION_CHANNEL, "  HASH_MARKS         : %d\n", HASH_MARKS );
  return( 0 );
}


ssize_t
my_read( int fd, void* buf, size_t nbtr ) {
/* returns number of bytes read or -1 on error */
/* like read(2) but nbr<nbtr only if eof reached */
  ssize_t nbr;
  ssize_t tnbr= 0;
  size_t rem= nbtr;
  unsigned char *p= (unsigned char*)buf;
  do {
    if( (nbr=read(fd,p+tnbr,rem)) == -1 ) return( -1 );
    tnbr+= nbr;
    rem= nbtr - tnbr;
  }
  while( nbr>1 && rem>0 );
  return( tnbr );
}


long
iso_num_733( char* p ) {
  return(   (p[0] & 0xffL)
          | (p[1] & 0xffL) <<  8
          | (p[2] & 0xffL) << 16
          | (p[3] & 0xffL) << 24 );
}


int
fra2min( int f ) {
/* convert frames to minutes of msf */
  f= f < 0 ? -f : f;
  return( f/CD_FRAMES/CD_SECS );
}


int
fra2sec( int f ) {
/* convert frames to seconds of msf */
  f= f < 0 ? -f : f;
  return( (f - fra2min(f)*CD_SECS*CD_FRAMES)/CD_FRAMES );
}

int
fra2fra( int f ) {
/* convert frames to frames of msf */
  f= f < 0 ? -f : f;
  return( f - (fra2min(f)*CD_SECS+fra2sec(f))*CD_FRAMES );
}

unsigned char*
setup_head( int blk, int uhb ) {
/* set up head data (4 bytes) */
  static unsigned char head[CD_HEAD_SIZE];
  static int min, sec, fra;
  min= fra2min( blk+CD_MSF_OFFSET );
  sec= fra2sec( blk+CD_MSF_OFFSET );
  fra= fra2fra( blk+CD_MSF_OFFSET );
  head[0]= (unsigned char) 16*(min/10)+min%10;
  head[1]= (unsigned char) 16*(sec/10)+sec%10;
  head[2]= (unsigned char) 16*(fra/10)+fra%10;
  head[3]= (unsigned char) uhb;
  return( head );
}


char*
basename( char* name ) {
/* strip directory from name */
/* basename("/usr/bin/") == "" -- basename(1) would return "bin" !! */
  char* p= name;
  while( *p != '\0' ) p++;
  while( p > name ) {
    if( *(p-1) == '/' ) break;
    else p--;
  }
  return( p );
}


int
main( int argc, char** argv ) {
/*
SOLYGA --------------------
SOLYGA main(argc, argv) isodump
SOLYGA extract iso9660 image from device or file
SOLYGA started Mon Jun 15 12:23:54 MET DST 1998 @beast
*/
  char* fpn= *argv;			/* full program name */
  char* pn= basename( fpn );		/* base program name */
  int retval= RETVAL_OK;		/* return value */
  int c;				/* option char */
  char stdin_fn[]= "stdin";
  char stdout_fn[]= "stdout";
  char default_in_fn[]= DEFAULT_INPUT_FILE;
  int in_fd= -1;			/* input fd */
  char* in_fn= default_in_fn;		/* input file name */
  int out_fd= STDOUT_FILENO;		/* output fd */
  char* out_fn= stdout_fn;		/* output file name */
  int verbose= 0;			/* verbosity level */
  int hash= 0;				/* hash marks ? */
  int all= 0;				/* write all data available */
  int xa= 0;				/* XA disk, block offset= 24 */
  int nrg= 0;				/* nrg image, skip first 150 blocks */
  int pad= 1;				/* pad image? */
  ssize_t nbr;				/* number of bytes read */
  long long tnbr= 0;			/* total number of bytes read */
  ssize_t nbw;				/* number of bytes written */
  long long tnbw= 0;			/* total number of bytes written */
  int blk;				/* current block */
  int blocks;				/* number of blocks to dump */
  time_t sec0, sec1;			/* speed measurement */
  unsigned char buf[CD_FRAMESIZE_RAW];	/* read buffer */
  unsigned char* bp;			/* start of iso-block */
  int buf_size;
  int hash_marks_printed= 0;
  unsigned char sync[CD_SYNC_SIZE]= {0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00};
  unsigned char head[CD_HEAD_SIZE]= {0x00,0x02,0x00,0x01};
  char id[]= ISO_STANDARD_ID;		/* "CD001" */
  int uhb= -1;				/* unidentified head byte */
  int extra_zero;			/* flag: extra data is zero */
  int padded;				/* flag & number of pad blocks */
  int i;

/* process options */

  *argv= pn;
  while( (c=getopt(argc,argv,"ahHnpvVx")) != EOF )
    switch( c ) {
      case 'a': /* write all data available */
        all= 1;
        break;
      case 'h': /* display help to HELP_CHANNEL and exit sucessfully */
        display_help( pn );
        retval= RETVAL_OK; goto DIE_NOW;
      case 'H': /* print hash marks */
        hash= 1;
        break;
      case 'n': /* nrg image, skip first 150 blocks */
        nrg= 1;
        break;
      case 'p':	/* don't pad image */
        pad= 0;
        break;
      case 'v': /* increase verbosity level */
        verbose++;
        break;
      case 'V': /* display version to VERSION_CHANNEL and exit sucessfully */
        display_version( pn );
        retval= RETVAL_OK; goto DIE_NOW;
      case 'x':	/* XA disk ==> offset=CD_XA_SYNC_HEAD */
        xa= 1;
        break;
      case '?': /* refer to help and exit unsuccessfully */
        fprintf( ERROR_CHANNEL, "%s: Try `%s -h' for more information.\n",
                 pn, pn );
        retval= RETVAL_ERROR; goto DIE_NOW;
      default : /* program error */
        fprintf( ERROR_CHANNEL, "%s: Options bug! E-mail me at %s.\n",
                 pn, MY_EMAIL_ADDRESS );
        retval= RETVAL_BUG; goto DIE_NOW;
    }

/* open input and output */

  if( argc > optind ) {
    if( *argv[optind] == '-' ) {
      in_fn= stdin_fn;
      in_fd= STDIN_FILENO;
    }
    else {
      in_fn= argv[optind];
      in_fd= -1;
    }
  }
  if( in_fd != STDIN_FILENO ) {
    if( (in_fd=open(in_fn,O_RDONLY)) == -1 ) {
      fprintf( ERROR_CHANNEL, "%s: Cannot open `%s' read-only. %s.\n",
               pn, in_fn, strerror(errno) );
      retval= RETVAL_ERROR; goto DIE_NOW;
    }
  }
  if( argc > optind+1 ) {
    if( *argv[optind+1] == '-' ) {
      out_fn= stdout_fn;
      out_fd= STDOUT_FILENO;
    }
    else {
      out_fn= argv[optind+1];
      out_fd= -1;
    }
  } 
  if( out_fd != STDOUT_FILENO ) {
    if( (out_fd=open(out_fn,O_WRONLY|O_CREAT|O_TRUNC,OUT_PERMS)) == -1 ) {
      fprintf( ERROR_CHANNEL, "%s: Cannot open `%s' write-only. %s.\n",
               pn, out_fn, strerror(errno) );
      retval= RETVAL_ERROR; goto DIE_NOW;
    }
  }

/* throw out some info */

  if( verbose >= 2 ) {
    if( in_fd == STDIN_FILENO )
      fprintf( VERBOSE_CHANNEL, "%s: Reading from stdin.\n", pn );
    else
      fprintf( VERBOSE_CHANNEL, "%s: Reading from file `%s'.\n", pn, in_fn );
    if( out_fd == STDOUT_FILENO )
      fprintf( VERBOSE_CHANNEL, "%s: Writing to stdout.\n", pn );
    else
      fprintf( VERBOSE_CHANNEL, "%s: Writing to file `%s'.\n", pn, out_fn );
    if( hash )
      fprintf( VERBOSE_CHANNEL,
               "%s: Writing hash marks (%d for full image size).\n",
               pn, HASH_MARKS );
  }

/* dump input file */

  blocks= ISO_PD_BLOCK + 1;
  bp= buf; buf_size= CD_FRAMESIZE;
  tnbr= 0; tnbw= 0;
  extra_zero= 1;
  padded= 0;
  sec0= time( NULL ); sec1= sec0;

  if( nrg ) {
    /* nrg image, skip first 150 blocks */
    if( verbose ) {
      fprintf( VERBOSE_CHANNEL,
               "%s: Assuming NRG image, skipping first %d blocks.\n",
               pn, NRG_BLOCKS );
    }
    for( blk=0; blk<NRG_BLOCKS; blk++ ) {
      if( (nbr=my_read(in_fd,buf,buf_size)) == -1 ) {
        fprintf( ERROR_CHANNEL, "%s: Read error at block %d. %s.\n",
                 pn, blk, strerror(errno) );
        retval= RETVAL_READ_ERROR; goto DIE_NOW;
      }
      tnbr+= nbr;
    }
  }

  for( blk=0; 1; blk++ ) {
    /* read block */
    if( (nbr=my_read(in_fd,buf,buf_size)) == -1 ) {
      fprintf( ERROR_CHANNEL, "%s: Read error at block %d. %s.\n",
               pn, blk, strerror(errno) );
      retval= RETVAL_READ_ERROR; goto DIE_NOW;
    }
    tnbr+= nbr;
    /* check for EOF */
    if( nbr < buf_size ) {	/* EOF reached */
      if( blk < blocks ) {	/* image too small, error or pad */
        if( !pad  ||  blk <= ISO_PD_BLOCK ) {
          fprintf( ERROR_CHANNEL,
                   "%s: Image too small by at least %d-%d= %d blocks.\n",
                   pn, blocks, blk, blocks-blk );
          retval= RETVAL_BROKEN; goto DIE_NOW;
        } else {
          padded= padded ? padded : blocks-blk;
          for( i=nbr; i<buf_size; i++ ) buf[i]= 0;
          nbr= buf_size;
        }
      } else {			/* image size okay */
        if( nbr == 0 ) break;
        for( i=nbr; i<buf_size; i++ ) buf[i]= 0;
        nbr= buf_size;
      }
    }
    /* virgin loop: check image type and set vars */
    if( blk == 0 ) {
      /* sync data present? */
      if( memcmp(bp,sync,CD_SYNC_SIZE) == 0 ) {
        int nbtr= CD_FRAMESIZE_RAW-CD_FRAMESIZE;
        buf_size= CD_FRAMESIZE_RAW;
        bp+= CD_SYNC_SIZE;
        if( (nbr=my_read(in_fd,buf+nbr,nbtr)) == -1 ) {
          fprintf( ERROR_CHANNEL, "%s: Read error at block %d. %s.\n",
                   pn, blk, strerror(errno) );
          retval= RETVAL_READ_ERROR; goto DIE_NOW;
        }
        tnbr+= nbr;
        if( nbr < nbtr ) {
          fprintf( ERROR_CHANNEL, "%s: Image broken -- too small.\n", pn );
          retval= RETVAL_BROKEN; goto DIE_NOW;
        }
      }
      /* subchannel data present? */
      if( memcmp(bp,head,CD_HEAD_SIZE-1) == 0 ) {
        uhb= bp[3];
        buf_size= CD_FRAMESIZE_RAW;
        bp+= CD_HEAD_SIZE;
      }
      if( xa ) {
        buf_size= CD_FRAMESIZE_RAW;
        bp= buf + CD_XA_SYNC_HEAD;
      }
      if( verbose >= 2 ) {
        fprintf( VERBOSE_CHANNEL, "%s: Input blocksize set to %d bytes.\n",
                 pn, buf_size );
        fprintf( VERBOSE_CHANNEL, "%s: Input blockoffset set to %d bytes.\n",
                 pn, bp-buf );
        if( buf_size > CD_FRAMESIZE )
          fprintf( VERBOSE_CHANNEL, "%s: Unidentified header byte is 0x%02x.\n",
                   pn, uhb );
      }
    }
    /* if raw (2352 bytes) image: check sync and head data */
    if( buf_size == CD_FRAMESIZE_RAW  &&  !padded ) {
      /* check sync data */
      if( memcmp(buf,sync,CD_SYNC_SIZE) != 0 ) {
        fprintf( ERROR_CHANNEL, "%s: Invalid sync data at block %d.\n",
                 pn, blk );
        retval= RETVAL_ERROR; goto DIE_NOW;
      }
      /* check head data */
      if( memcmp(buf+CD_SYNC_SIZE,setup_head(blk,uhb),CD_HEAD_SIZE) != 0 ) {
        fprintf( ERROR_CHANNEL, "%s: Invalid head data at block %d.\n",
                 pn, blk );
        retval= RETVAL_ERROR; goto DIE_NOW;
      }
    }
    /* write iso9660-block */
    if( blk<blocks || all ) {
      if( (nbw=write(out_fd,bp,CD_FRAMESIZE)) != CD_FRAMESIZE ) {
        fprintf( ERROR_CHANNEL, "%s: Write error at block %d. %s.\n",
                 pn, blk, strerror(errno) );
        retval= RETVAL_ERROR; goto DIE_NOW;
      }
      tnbw+= nbw;
    }
    /* check extra data */
    if( blk >= blocks ) {
      for( i=0; i<CD_FRAMESIZE; i++ )
        if( bp[i] != 0 ) {
          extra_zero= 0;
          break;
        }
    }
    /* set image size (blocks) and iso9660-magic data */
    if( blk == ISO_PD_BLOCK ) {
      struct iso_primary_descriptor* ipd= (struct iso_primary_descriptor*) bp;
      /* check iso9660-magic */
      if( memcmp(ipd->id,id,strlen(id)) != 0 ) {
        ipd= (struct iso_primary_descriptor*) (bp+CD_SUBHEAD_SIZE);
        if( memcmp(ipd->id,id,strlen(id)) == 0 ) {
          fprintf( ERROR_CHANNEL,
          "%s: XA image detected. Use the XA disk option.\n",
          pn );
        }
        else { /* nero or unknown image */
          int blk_zero= 1;
          for( i=0; i<CD_FRAMESIZE; i++ )
            if( bp[i] != 0 ) {
              blk_zero= 0;
              break;
            }
          if( blk_zero  &&  !nrg ) {
            fprintf( ERROR_CHANNEL,
            "%s: Unknown image type. Try the nrg image option.\n",
            pn );
          } else {
            fprintf( ERROR_CHANNEL, "%s: Unknown image type. Exiting.\n", pn );
          }
        }
        retval= RETVAL_BROKEN; goto DIE_NOW;
      }
      /* set and check blocks */
      blocks= iso_num_733(ipd->volume_space_size);
      if( verbose) {
        fprintf( VERBOSE_CHANNEL, "%s: Image has %d blocks, ", pn, blocks );
        fprintf( VERBOSE_CHANNEL, "%lld bytes, %lld Mbytes, ",
                 blocks*(long long)CD_FRAMESIZE,
                 blocks*(long long)CD_FRAMESIZE/1024/1024 );
        fprintf( VERBOSE_CHANNEL, "%02d:%02d:%02d msf.\n",
                 fra2min(blocks), fra2sec(blocks), fra2fra(blocks) );
      }
      if( blocks <= blk+1 ) {
        fprintf( ERROR_CHANNEL,
                 "%s: Image (primary descriptor block) broken. Exiting.\n",
                 pn );
        retval= RETVAL_BROKEN; goto DIE_NOW;
      }
    }
    /* write has mark(s) */
    if( hash && blk>=ISO_PD_BLOCK ) {
      int hash_marks_to_print= ((blk+1)*HASH_MARKS)/blocks - hash_marks_printed;
      for( i=0; i<hash_marks_to_print; i++ ) fprintf( VERBOSE_CHANNEL, "#" );
      hash_marks_printed+= hash_marks_to_print;
    }
  }
  sec1= time( NULL );
  if( hash ) fprintf( VERBOSE_CHANNEL, "\n" );
  if( padded ) {
    fprintf( ERROR_CHANNEL,
             "%s: WARNING: Image may be broken, padded by %d blocks!\n",
             pn, padded );
    retval= RETVAL_PADDED;
  }
  if( verbose && blk>blocks) {
    if( all ) {
      fprintf( VERBOSE_CHANNEL,
               "%s: Extra data written (%d blocks, %s).\n",
               pn, blk-blocks, extra_zero?"all zero":"non-zero" );
    }
    else {
      fprintf( VERBOSE_CHANNEL,
               "%s: Extra data cut (%d blocks, %s).\n",
               pn, blk-blocks, extra_zero?"all zero":"non-zero" );
    }
  }

/* close output and input */

  if( out_fd != STDOUT_FILENO  &&  close(out_fd) == -1 ) {
    fprintf( ERROR_CHANNEL, "%s: Cannot close output file `%s'. %s.\n",
             pn, out_fn, strerror(errno) );
    retval= RETVAL_ERROR;
  }
  if( in_fd != STDIN_FILENO  &&  close(in_fd) == -1 ) {
    fprintf( ERROR_CHANNEL, "%s: Cannot close input file `%s'. %s.\n",
             pn, in_fn, strerror(errno) );
    retval= RETVAL_ERROR;
  }

/* give some more info */
  if( verbose ) {
    /* print overall speed */
    double speed_abs, speed_rel;
    char unit[]= " "; *unit= '\0';
    speed_abs= (double)(sec1==sec0 ? 1.048565e8 : tnbw/(double)(sec1-sec0));
    speed_rel= speed_abs/(double)(CD_FRAMESIZE*CD_FRAMES);
    if( speed_abs > 1024.0 ) { speed_abs /= 1024.0; unit[0]= 'k'; }
    if( speed_abs > 1024.0 ) { speed_abs /= 1024.0; unit[0]= 'M'; }
    fprintf( VERBOSE_CHANNEL, "%s: Wall clock speed %5.3f %sbytes/s, %3.1fx.\n",
             pn, speed_abs, unit, speed_rel );
  }

DIE_NOW:
  close( out_fd );
  close( in_fd );
  if( verbose )
    fprintf( VERBOSE_CHANNEL, "%s: %lld/%lld bytes read/written.\n",
             pn, tnbr, tnbw );
  return( retval );
}
