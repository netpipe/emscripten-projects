/*
 *	read raw cd data track (test)
 *
 *	with ide-cd.c v4.54
 *	(grep IDECD_VERSION /usr/src/linux/drivers/block/ide-cd.c)
 *	this program only works with the patch ide-cd.c-v4.54.diff
 */

/*
 *	$Id: readraw.c,v 1.7 2000-09-13 00:40:29 solyga Exp $
 */

#include	"../isodump.h"
#include	<linux/cdrom.h>
#include	<sys/ioctl.h>
#define	DEBUG
#undef	DEBUG
#define	RETVAL_MALLOC	6

#define	LBA2MSF( LBA0, LBA1, MSF )	\
	MSF.cdmsf_min0= (LBA0+CD_MSF_OFFSET) / (CD_SECS*CD_FRAMES); \
	MSF.cdmsf_sec0= ((LBA0+CD_MSF_OFFSET) % (CD_SECS*CD_FRAMES)) / CD_FRAMES; \
	MSF.cdmsf_frame0= (LBA0+CD_MSF_OFFSET) % CD_FRAMES; \
	MSF.cdmsf_min1= (LBA1+CD_MSF_OFFSET) / (CD_SECS*CD_FRAMES); \
	MSF.cdmsf_sec1= ((LBA1+CD_MSF_OFFSET) % (CD_SECS*CD_FRAMES)) / CD_FRAMES; \
	MSF.cdmsf_frame1= (LBA1+CD_MSF_OFFSET) % CD_FRAMES

#define	MSF2LBA( LBA0, LBA1, MSF ) \
	LBA0= ( MSF.cdmsf_min0 * CD_SECS + MSF.cdmsf_sec0 ) * CD_FRAMES + \
		MSF.cdmsf_frame0 - CD_MSF_OFFSET; \
	LBA1= ( MSF.cdmsf_min1 * CD_SECS + MSF.cdmsf_sec1 ) * CD_FRAMES + \
		MSF.cdmsf_frame1 - CD_MSF_OFFSET

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
main(int argc, char** argv) {
  char* fpn= *argv;
  char* pn= basename( fpn );
  int retval= RETVAL_OK;
  int verbose= 2;
  char default_cd_fn[]= DEFAULT_INPUT_FILE;
  char* cd_fn= default_cd_fn;
  int cd_fd;
  struct cdrom_tochdr header;
  int trk0= 0;		/* first track */
  int trk1= 0;		/* last track +1 (leadout) */
  int trks= 0;		
  int trk= 0;
  int ti= 0;		/* track index */
  int blk0= 0;		/* index of fist block */
  int blk1= 0;		/* index of last block +1 */
  int blks= 0;
  int blk;
  struct cdrom_tocentry* entry= NULL;
  int i;
  UCHAR buf[CD_FRAMESIZE_RAW];
  struct cdrom_msf msf;

#ifdef	DEBUG
    fprintf( DEBUG_CHANNEL, "DEBUG: sizeof(struct cdrom_read)= %d bytes\n", sizeof(struct cdrom_read) );
    fprintf( DEBUG_CHANNEL, "DEBUG: sizeof(struct cdrom_msf)= %d bytes\n", sizeof(struct cdrom_msf) );
    fprintf( DEBUG_CHANNEL, "DEBUG: sizeof(int)= %d bytes\n", sizeof(int) );
    fprintf( DEBUG_CHANNEL, "DEBUG: sizeof(caddr_t)= %d bytes\n", sizeof(caddr_t) );
    fprintf( DEBUG_CHANNEL, "DEBUG: &buf= %08x\n", buf );
#endif


/* open input */
  if( optind < argc ) cd_fn= argv[optind];
  if( (cd_fd=open(cd_fn,O_RDONLY,0|O_NONBLOCK)) == -1 ) {
    fprintf( ERROR_CHANNEL, "%s: Couldn't open `%s' for reading. %s.\n",
             pn, cd_fn, sys_errlist[errno] );
    retval= RETVAL_ERROR; goto DIE_NOW;
  }


/* get toc header */
  if( verbose >= 2 )
    fprintf( VERBOSE_CHANNEL, "%s: Reading table of contents.\n", pn );
  if( ioctl(cd_fd,CDROMREADTOCHDR,&header) == -1 ) {
    fprintf( ERROR_CHANNEL, "%s: Couldn't get toc header. %s.\n",
             pn, sys_errlist[errno] );
    retval= RETVAL_ERROR; goto DIE_NOW;
  }
  trk0= (int) header.cdth_trk0;
  trk1= (int) header.cdth_trk1 + 1;
  trks= trk1 - trk0;
  if( verbose >= 2 )
    fprintf( VERBOSE_CHANNEL, "%s: Cd contains tracks [%d,%d)\n",
             pn, trk0, trk1 );


/* get toc entries */
  if( (entry=malloc((trks+1)*sizeof(struct cdrom_tocentry))) == NULL ) {
    fprintf( ERROR_CHANNEL, "%s: Couldn't read toc. %s.\n",
             pn, sys_errlist[errno] );
    retval= RETVAL_MALLOC; goto DIE_NOW;
  }
  for( ti=0; ti<=trks; ti++ ) {
    entry[ti].cdte_track= ti<trks ? trk0+ti : CDROM_LEADOUT;
    entry[ti].cdte_format = CDROM_LBA;
    if( ioctl(cd_fd,CDROMREADTOCENTRY,&entry[ti]) == -1 ) {
      fprintf( ERROR_CHANNEL, "%s: Couldn't read toc. %s.\n",
               pn, sys_errlist[errno] );
      retval= RETVAL_ERROR; goto DIE_NOW;
    }
  }


/* set track index ti and block indices blk0, blk1 */
  trk= trk0;
  trk=27;
  if( verbose )
    fprintf( VERBOSE_CHANNEL, "%s: Trying to raw read track %d.\n", pn, trk );
  for( ti=0; ti<=trks; ti++ )
    if( entry[ti].cdte_track == trk ) break;
  if( ti >= trks ) {
    fprintf( ERROR_CHANNEL, "%s: CD doesn't contain track %d.\n", pn, trk );
    retval= RETVAL_ERROR; goto DIE_NOW;
  }
  blk0= entry[ti  ].cdte_addr.lba;
  blk1= entry[ti+1].cdte_addr.lba;


/* check for trk beeing data track */
  if( ! (entry[ti].cdte_ctrl & CDROM_DATA_TRACK) ) {
    fprintf( ERROR_CHANNEL, "%s: Warning: Track %d is not a data track!\n",
             pn, trk );
/*
    retval= RETVAL_ERROR; goto DIE_NOW;
*/
  }


/* raw copy blocks */
  for( blk=blk0; blk<blk1; blk++ ) {
    int lba0= blk;
    int lba1= 0;
    LBA2MSF( lba0, lba1, msf );
    memcpy( buf, &msf, sizeof(msf) );
    if( ioctl(cd_fd,CDROMREADRAW,buf) == -1 ) {
/*
      int try= 5;
      fprintf( ERROR_CHANNEL, "%s: Problem with block %d. %s\n",
               pn, blk, sys_errlist[errno] );
      while( try > 0 ) {
        if( ioctl(cd_fd,CDROMREADRAW,&arg) != -1 ) break;
        try--;
      }
      if( try == 0 ) {
*/
        fprintf( ERROR_CHANNEL, "%s: Cannot read block %d. %s\n",
                 pn, blk, sys_errlist[errno] );
        retval= RETVAL_ERROR; goto DIE_NOW;
/*
      }
*/
    }
    /* don't skip the control bytes, maybe we're reading audio... */
/*
    write( STDOUT_FILENO, buf, CD_FRAMESIZE_RAW );
*/
    /* skip control bytes */
    /* this works fine for CD+ */
    write( STDOUT_FILENO, buf+CD_XA_SYNC_HEAD, CD_FRAMESIZE );
  }


DIE_NOW:
  exit( retval );
}
