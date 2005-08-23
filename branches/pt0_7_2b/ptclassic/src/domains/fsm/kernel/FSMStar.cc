static const char file_id[] = "FSMStar.cc";

/*  Version @(#)FSMStar.cc	1.3 12/08/97

@Copyright (c) 1996-1997 The Regents of the University of California.
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

    Author:	Bilung Lee
    Created:	3/3/96

*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "FSMStar.h"

// Define firing
int FSMStar :: run() {
        int i;
	BlockPortIter next(*this);
	for(i = numberPorts(); i > 0; i--)
		(next++)->receiveData();
	int status = Star::run();
	// we send the data even on error
	next.reset();
	for(i = numberPorts(); i > 0; i--)
		(next++)->sendData();
	return status;
}


extern const char FSMdomainName[];

// Class identification.
ISA_FUNC(FSMStar,Star);

// Domain identification.
const char* FSMStar::domain() const
{
    return FSMdomainName;
}

