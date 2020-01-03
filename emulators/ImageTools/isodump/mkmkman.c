/*
  mkmkman.c  --  make mkman.c
  Copyright (C) 1999 Steffen Solyga <solyga@absinth.net>

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

/*#include	"header.h"*/	/* all prg defs must be in this header file */
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#define		MKMKMAN_BUFFER_SIZE	32

/*
  - reads manual template from stdin, writes mkman.c (which will generate
    the man file later) to stdout
  - if a substring "[FORMAT DEF_NAME]" is found in the template it will finally 
    be replaced by the value of the constant DEF_NAME, where FORMAT is used as
    output format for printf(). Example:
    header.h:		#define	BUFFER_SIZE	128
    gcc command line:	... -Dprg_name=\"nice-program\"
    template.man:	... [%s prg_name] ... [%d BUFFER_SIZE] bytes ...
    ==> nice-program.1:	... nice-program ... 128 bytes ...
  - DEF_NAME must not contain a ']'
  ==> "[%" indicates the substring to replace and cannot be used in the manual
  - in the manual template all defs from header.h and the cmd-line can be used,
    in addition the following constants may be used:
    - Prg_name			(fist letter uppercase)
    - PRG_NAME			(all letters uppercase)
    - MANUAL_SECTION_STR
    - STD_CHANNEL_STR		(ifdef STD_CHANNEL)
    - HELP_CHANNEL_STR		(ifdef HELP_CHANNEL)
    - VERSION_CHANNEL_STR	(ifdef VERSION_CHANNEL)
    - VERBOSE_CHANNEL_STR	(ifdef VERBOSE_CHANNEL)
    - ERROR_CHANNEL_STR		(ifdef ERROR_CHANNEL)
  - mkmkman.c needs cmd-line definition of
    - prg_name
    - MANUAL_NUMBER
    - OS_TYPE_STR

last modified: Sat Apr 3 11:52:31 CEST 1999 @beast

*/


char*
read_line( char* s ) {
  static size_t size= MKMKMAN_BUFFER_SIZE;
  size_t nbr= 0;
  int c;
  if( s == NULL ) s= (char*)malloc(size);
  if( s == NULL ) return( NULL );
  while( (c=getchar()) != EOF ) {
    s[nbr++]= (char)c;
    if( c == '\n' ) {
      s[nbr-1]= '\0';	/* string end */
      s[nbr]=   '\0';	/* buffer end */
      break;
    }
    if( nbr+1 >= size ) {	/* +1 for buffer end */
      size+= MKMKMAN_BUFFER_SIZE;
      s= realloc( s, size );
      if( s == NULL ) return( NULL );
    }
  }
  if( nbr == 0 ) return( NULL );
  return( s );
}


int
output_normal_printf( char* s ) {
  char* p= s;
  if( *s == '\0' ) return( 0 );
  printf( "  printf( \"" );
  while( *p != '\0' ) {
    switch( *p ) {
      case '"' : printf( "\\%c", *p ); break;
      case '\\': printf( "\\%c", *p ); break;
      case '%' : printf( "%%%c", *p ); break;
      default  : printf( "%c", *p );
    }
    p++;
  }
  printf( "\" );\n" );
  return( 0 );
}


int
main( int argc, char** argv ) {
  int retval= 0;
  char* line_str= NULL;
  int line= 0;
  char actual_prg_name[]= prg_name;	/* prg_name: cc cmd-line def */
  char* p= actual_prg_name;

/* output header stuff */
  printf( "/*\n" );
  printf( "  mkman.c  --  output manual page for %s\n", prg_name );
  printf( "  generated automatically by %s\n", *argv );
  printf( "  Copyright (C) 1999 Steffen Solyga <solyga@absinth.net>\n" );
  printf( "*/\n" );
  printf( "\n" );
  printf( "#include\t\"header.h\"\n" );
  printf( "#include\t<stdio.h>\n" );
  printf( "\n" );
  /* program name(s) */
  *p= (char)toupper((int)*p);
  printf( "#define\tPrg_name\t\"%s\"\n", actual_prg_name );
  while( *p != '\0' ) { *p= (char)toupper((int)*p); p++; }
  printf( "#define\tPRG_NAME\t\"%s\"\n", actual_prg_name );
  /* channels */
  printf( "#ifdef\tHELP_CHANNEL\n" );
  printf( "#define\tHELP_CHANNEL_STR\t(HELP_CHANNEL==stdout?\"stdout\":\"stderr\")\n" );
  printf( "#endif\n" );
  printf( "#ifdef\tSTD_CHANNEL\n" );
  printf( "#define\tSTD_CHANNEL_STR\t(STD_CHANNEL==stdout?\"stdout\":\"stderr\")\n" );
  printf( "#endif\n" );
  printf( "#ifdef\tVERSION_CHANNEL\n" );
  printf( "#define\tVERSION_CHANNEL_STR\t(VERSION_CHANNEL==stdout?\"stdout\":\"stderr\")\n" );
  printf( "#endif\n" );
  printf( "#ifdef\tVERBOSE_CHANNEL\n" );
  printf( "#define\tVERBOSE_CHANNEL_STR\t(VERBOSE_CHANNEL==stdout?\"stdout\":\"stderr\")\n" );
  printf( "#endif\n" );
  printf( "#ifdef\tERROR_CHANNEL\n" );
  printf( "#define\tERROR_CHANNEL_STR\t(ERROR_CHANNEL==stdout?\"stdout\":\"stderr\")\n" );
  printf( "#endif\n" );
  /* manual section */
  printf( "#define\tMANUAL_SECTION_STR\t\"%s ", OS_TYPE_STR );
  switch( MANUAL_NUMBER ) {
    case  1: printf( "User's " ); break;
    case  2:
    case  3: printf( "Programmer's " ); break;
    case  8: printf( "System Manager's " ); break;
    default: ;
  }
  printf( "Manual\"\n" );
  printf( "\n" );
  /* main() header */
  printf( "int main( int argc, char** argv ) {\n" );
  printf( "  int retval= 0;\n" );

/* output body stuff (man page contents) */
  while( (line_str=read_line(line_str)) != NULL ) {
    char* s= line_str;
    char* p;
    line++;
    while( (p=strstr(s,"[%")) != NULL ) {
      char* pend;
      char* fmt;
      if( (pend=strstr(p,"]")) == NULL ) {
        fprintf( stderr, "%s: Missing `]' (input line %d)\n",
                 *argv, line );
        retval= 1; goto DIE_NOW;
      }
      *p= '\0'; p++;			/* delete '[', *p == '%' */
      *pend= '\0';			/* delete ']' */
      output_normal_printf( s );
      s= pend+1;			/* s -> behind ']' (now '\0') */
      if( (pend=strstr(p," ")) == NULL ) {
        fprintf( stderr, "%s: Missing space in `%s' (input line %d)\n",
                 *argv, p, line );
        retval= 1; goto DIE_NOW;
      }
      *pend= '\0';			/* delete ' ' */
      fmt= p;
      p= pend+1;			/* p -> DEF_NAME */
      printf( "  printf( \"%s\", %s );\n", fmt, p );
    }
    output_normal_printf( s );
    printf( "  printf( \"\\n\" );\n" );
  }

/* output tail stuff */
DIE_NOW:
  printf( "  exit( retval );\n" );
  printf( "}\n" );
  free( line_str );
  exit( retval );
}
