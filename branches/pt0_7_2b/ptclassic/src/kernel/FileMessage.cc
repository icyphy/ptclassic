static const char file_id[] = "FileMessage.cc";
/**************************************************************************
Version identification:
$Id$

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

 Programmer:  Edward A. Lee
 Date of creation: 1/13/96

 This file defines the implementation of the FileMessage class and

**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "FileMessage.h"
#include "Error.h"
#include "miscFuncs.h"
#include "InfString.h"

/////////////////////////////////////////////////////////////
// FileMessage method bodies
/////////////////////////////////////////////////////////////

// Constructor: make a FileMessage with a unique filename
FileMessage::FileMessage() {
    filename = tempFileName();
    dynamicFilename = 1;
    transientFile = 1;
}

// Constructor: make a FileMessage with the given filename
FileMessage::FileMessage(const char* name) {
    filename = name;
    dynamicFilename = 0;
    transientFile = 0;
}

// Copy Constructor
FileMessage::FileMessage(const FileMessage& src) {
    filename = src.fileName();
    dynamicFilename = 0;
    transientFile = 0;
}

// Destructor
FileMessage::~FileMessage() {
    if (filename) {
	if (transientFile) {
	    InfString cmd = "rm -f ";
	    cmd << filename;
	    system(cmd);
	}
	if(dynamicFilename) {
	    delete [] filename;
	}
    }
}

// Return the filename as a StringList.
StringList FileMessage::print() const {
    StringList sl = fileName();
    return sl;
}
