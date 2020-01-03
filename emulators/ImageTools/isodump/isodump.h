/*
 * $Id: isodump.h,v 1.24 2008-01-30 22:27:09 solyga Exp $
 */

#define	DEBUG
#undef	DEBUG

#define	_LARGEFILE64_SOURCE
#define	_FILE_OFFSET_BITS	64

#include <stdio.h>		/* printf() */
#include <stdlib.h>		/* malloc() */
#include <getopt.h>		/* getopt() */
#include <unistd.h>		/* read(), write(), close(), STDIN_FILENO */
#include <errno.h>		/* error numbers */
#include <sys/types.h>		/* open() */
#include <sys/stat.h>		/* open() */
#include <fcntl.h>		/* open(), O_RDONLY */
#include <string.h>		/* strlen() */
#include <time.h>		/* time() */
#include <linux/cdrom.h>	/* CD_* definitions */
#include "iso9660.h"		/* iso9660 structures & definitions (mkisofs)*/

typedef	unsigned char	UCHAR;

#define	VERBOSE_CHANNEL	stderr
#define	HELP_CHANNEL	stdout
#define	ERROR_CHANNEL	stderr
#define	VERSION_CHANNEL	stdout
#define	DEBUG_CHANNEL	stderr

#define	DEFAULT_INPUT_FILE	"/dev/cdrom"
#define	BUF_SIZE		((ISO_PD_BLOCK+1)*CD_FRAMESIZE_RAW)
#define	OUT_PERMS		S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH
#define	HASH_MARKS		80	/* until volume space size dumped */

#define	ISO_PD_BLOCK	16		/* offset, PD = primary desciptor */
#define	NRG_BLOCKS	150		/* blocks to skip for nrg images */

#define	RETVAL_OK			0
#define	RETVAL_PADDED			1
#define	RETVAL_BUG			2
#define	RETVAL_READ_ERROR		3
#define	RETVAL_BROKEN			4
#define	RETVAL_ERROR			5

#define	VERSION_NUMBER		"0.06.00"
#define	DATE_OF_LAST_MOD	"2008-01-30"
#define	MY_EMAIL_ADDRESS	"Steffen Solyga <solyga@absinth.net>"
