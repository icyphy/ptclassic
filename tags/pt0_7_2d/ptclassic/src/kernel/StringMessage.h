#ifndef _StringMessage_h
#define _StringMessage_h 1

#ifdef __GNUG__
#pragma interface
#endif
/**************************************************************************
Version identification:
@(#)StringMessage.h	1.2	2/27/96

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
#include "Message.h"
#include "InfString.h"

extern const DataType STRING;

/////////////////////////////////////////////////
// Common base for derived StringMessage Message classes
/////////////////////////////////////////////////
class StringMessage : public Message {
 public:

    /////////////////////////////////////////////////////////////////////
    //                    constructors and destructors                 //

    // Constructor that creates an empty string.
    StringMessage();

    // Constructor copies a specified string (you may delete the original).
    StringMessage(const char* name);

    // Copy constructor.
    StringMessage(const StringMessage& src);

    // Destructor
    ~StringMessage();

    /////////////////////////////////////////////////////////////////////
    //                        public methods                           //

    // Required method redefinitions from base class
    const char* dataType() const { return "String"; }
    Message* clone() const { return new StringMessage(*this); }
    ISA_INLINE(StringMessage,Message);

    // Return the name of the file as a StringList
    /* virtual */ StringList print() const;

    /////////////////////////////////////////////////////////////////////
    //                        protected variables                      //

 protected:
    InfString value;
};

#endif
