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
/* $Id$
   Copyright 1992 The Regents of the University of California
   All Rights Reserved.

   Author:	T. M. Parks
   Date:	8 October 1992

*/

#ifndef _CodeStream_h
#define _CodeStream_h

#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"
#include "UniqueStringList.h"

class CodeStream : public StringList
{
public:
    // Add code to the stream.  If "name" is not NULL code is added
    // only if "name" has not appeared before.
    // Returns TRUE if code was added to the stream, FALSE otherwise.
    int put(const char* code, const char* name = NULL);

    // initialize myself and sharedNameds if exists.
    void initialize();

protected:
    UniqueStringList sharedNames;
};

#endif
