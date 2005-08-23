static const char file_id[] = "StringMessage.cc";
/**************************************************************************
Version identification:
@(#)StringMessage.cc	1.3	2/29/96

Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer:  Edward A. Lee, John Reekie
 Date of creation: 1/13/96

 This file defines the implementation of the StringMessage class.

**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "StringMessage.h"
#include "Error.h"
#include "miscFuncs.h"

/////////////////////////////////////////////////////////////
// StringMessage method bodies
/////////////////////////////////////////////////////////////

// Constructor: make a StringMessage with an empty string.
StringMessage::StringMessage() {
    value = "";
}

// Constructor: make a StringMessage with the given string value.
// The string is copied, so the original may be deleted.
StringMessage::StringMessage(const char* name) {
    value = name;
}

// Copy Constructor
StringMessage::StringMessage(const StringMessage& src) {
    value = src.print();
}

// Destructor
StringMessage::~StringMessage() {
}

// Return the filename as a StringList.
StringList StringMessage::print() const {
    StringList ret;
    ret = value;
    return ret;
}
