static const char file_id[] = "ACSCorona.cc";
/**********************************************************************
Copyright (c) 1998 The Regents of the University of California.
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


 Programmers:  Eric Pauer (Sanders), Christopher Hylands
 Date of creation: 1/15/98
 Version: $Id$

***********************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ACSCorona.h"

// The following is defined in ACSDomain.cc -- this forces that module
// to be included if any ACS stars are linked in.
extern const char ACSdomainName[];

const char* ACSCorona :: domain () const { return ACSdomainName;}

// isA
ISA_FUNC(ACSCorona, ACSStar);

// select Core to be used
int ACSCorona::setCore(const char *coreName)
{
  ListIter iterator(coreList);
  // iterate through list until Core is found, if not return null
  return 0;
}

// register Core in the list
int ACSCorona::registerCore(ACSCore &core, const char* implementationName)
{
  // store pointer to core in the CoreList
  return 0;
}
