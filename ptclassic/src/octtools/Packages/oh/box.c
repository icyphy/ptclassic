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
#include "oct.h"

void ohBoxCenter( box, point )
    struct octBox *box;
    struct octPoint *point;
{
    point->x = (box->lowerLeft.x + box->upperRight.x) / 2;
    point->y = (box->lowerLeft.y + box->upperRight.y) / 2;
}


octCoord ohBoxWidth( box )
    struct octBox *box;
{
    return box->upperRight.x - box->lowerLeft.x;
}

octCoord ohBoxHeight( box )
    struct octBox *box;
{
    return box->upperRight.y - box->lowerLeft.y;
}

octCoord ohBoxArea( box )
    struct octBox *box;
{
    return ( box->upperRight.x - box->lowerLeft.x ) *
      (box->upperRight.y - box->lowerLeft.y );

}

