/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/
/* Version $Id$
   Copyright 1992 The Regents of the University of California
   All Rights Reserved.

   Author:      T. M. Parks
   Date:        4 October 1992

*/

static const char* file_id = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "UniqueStringList.h"

// Determine if a string is already present.
// If it is, remember it and return FALSE.
int UniqueStringList::isUnique(const char* s) 
{
    StringListIter string(*this);
    const char* u;

    // Search the list.
    while ((u = string++) != NULL)
    {
	if (strcmp(s,u) == 0)	// String is already in the list.
	    return FALSE;
    }

    // Add string to the list.
    *this += s;
    return TRUE;
}
