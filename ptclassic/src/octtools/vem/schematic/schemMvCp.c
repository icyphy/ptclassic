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
 * Schematic Move and Copy
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * This is an interface built on top of symbolic for schematic
 * move and copy.
 */

#include "general.h"
#include "oct.h"
#include "commands.h"
#include "se.h"

vemStatus schemMove(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
/*
 * This routine uses the symbolic move and copy except no
 * reconnects are ever done.
 */
{
    return seBaseMove(spot, cmdList, 0);
}

vemStatus schemCopy(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
/*
 * This routine uses the symbolic copy and copy except no
 * reconnects are ever done.
 */
{
    return seBaseCopy(spot, cmdList, 0, symDefConFunc);
}
