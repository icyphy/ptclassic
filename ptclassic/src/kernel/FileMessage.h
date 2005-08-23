#ifndef _FileMessage_h
#define _FileMessage_h 1

#ifdef __GNUG__
#pragma interface
#endif
/**************************************************************************
Version identification:
@(#)FileMessage.h	1.3	2/3/96

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

 Programmer:  Edward A. Lee
 Date of creation: 1/13/96

 This file defines the implementation of the FileMessage class.
 A FileMessage is a message that stores it data in a file on the
 host file system.

**************************************************************************/
#include "Message.h"

extern const DataType FILEMSG;

// FIXME: Want to be able to specify filename extensions so that tycho
// editors will understand.

/////////////////////////////////////////////////
// Common base for derived FileMessage Message classes
/////////////////////////////////////////////////
class FileMessage : public Message {
 public:

    /////////////////////////////////////////////////////////////////////
    //                    constructors and destructors                 //

    // Constructor the generates a new, unique filename
    FileMessage();

    // Constructor uses the specified filename.
    FileMessage(const char* name);

    // Copy constructor
    FileMessage(const FileMessage& src);

    // Destructor
    ~FileMessage();

    /////////////////////////////////////////////////////////////////////
    //                        public methods                           //

    // Required method redefinitions from base class
    const char* dataType() const { return "FileMessage"; }
    Message* clone() const { return new FileMessage(*this); }
    ISA_INLINE(FileMessage,Message);

    // Return the filename used by this message
    const char* fileName() const { return filename; }

    // Return the name of the file as a StringList
    /* virtual */ StringList print() const;

    // With non-zero argument, remove the file when the message is deleted.
    void setTransient(int transient) { transientFileFlag = transient; }

    /////////////////////////////////////////////////////////////////////
    //                        protected variables                      //

 protected:
    const char* filename;

    /////////////////////////////////////////////////////////////////////
    //                        protected variables                      //

 private:
    char* dynamicFilename;
    int transientFileFlag;
};

#endif
