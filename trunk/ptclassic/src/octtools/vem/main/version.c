#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/*
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
/*
 * VEM Version File
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986
 *
 * This file maintains the current version for VEM.  The version consists
 * of a major revision number,  a minor revision number,  and a date.
 * The major and minor revision string is expected to be passed in
 * the pre-processor variable CUR_VER.  The date is expected to be 
 * passed in as the pre-processor variable `CUR_DATE'.
 *
 * The module exports a function for returning the static version string
 * and a command for querying this version at run-time.
 */

static char *vemVersion = (char *) 0;

#include "general.h"
#include "commands.h"
#include "message.h"
#include "list.h"

static char *proc_date(datestr)
char *datestr;
/*
 * Returns the date in a brief format assuming its coming from
 * the program `date'.
 */
{
    static char result[25];
    char day[10], month[10], zone[10];
    int date, hour, minute, second, year;

    if (sscanf(datestr, "%s %s %2d %2d:%2d:%2d %s %4d",
	       day, month, &date, &hour, &minute, &second, zone, &year) == 8)
      {
	  sprintf(result, "%d-%3s-%02d", date, month, year % 100);
	  return result;
      }
    else return datestr;
}

#ifdef SABER
#define CUR_DATE	"Sat Oct  3 15:18:58 PDT 1987"
#endif

char *vemVerString()
/*
 * Returns the current VEM version string to the caller.
 */
{
    char temp[1024];

    if (!vemVersion) {
	sprintf(temp, "%s.Z (made %s)", CUR_VER, proc_date(CUR_DATE));
	vemVersion = VEMSTRCOPY(temp);
    }
    return vemVersion;
}

/*ARGSUSED*/
vemStatus vemVersionCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * Reports the version string for VEM in the console window.
 */
{
    char *rpcVerString();

    sprintf(errMsgArea, "This is VEM version %s, RPC library %s\n",
	    vemVerString(), rpcVerString());
    vemMessage(errMsgArea, MSG_DISP);
    return VEM_ARGRESP;
}
