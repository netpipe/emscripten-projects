/******************************* LICENCE **************************************
* Any code in this file may be redistributed or modified under the terms of
* the GNU General Public Licence as published by the Free Software 
* Foundation; version 2 of the licence.
****************************** END LICENCE ***********************************/

/******************************************************************************
* Author:
* Andrew Smith, http://littlesvr.ca/misc/contactandrew.php
*
* Contributors:
* 
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "isomaster.h"

void fatalError(const char* msg)
{
    fprintf(stderr, "ISO Master fatal error: %s\n", msg);
    exit(1);
}

void printWarning(const char* msg)
{
    fprintf(stderr, "ISO Master warning: %s\n", msg);
}
