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
/* $Id$
   Copyright 1993 The Regents of the University of California
   All rights reserved.

   Author:	Jose L. Pino
   Date:	October 15,1993
*/

static const char file_id[] = "CodeStreamList.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "CodeStreamList.h"

/*virtual*/ int CodeStreamList::remove(const char* name) {
        int status = NamedList::remove(name);
 	CodeStream* code = myStreams.get(name);
	if (code) {
		LOG_DEL; delete code;
		return myStreams.remove(name);
	}
	else {
		return status;
	}
}

CodeStream* CodeStreamList::newStream(const char* name) {
	CodeStream* code = get(name);
	if (!code) {
		LOG_NEW; code = new CodeStream;
		if (code) {
			append(code,name);
			myStreams.append(code,name);
		}
	}
	return code;
}

void CodeStreamList::deleteStreams() {
	ListIter nextNode(myStreams);
	NamedNode* node;
	while((node = (NamedNode*)nextNode++) != 0) {
		remove(node->name());
	}
}

void CodeStreamList::initialize() {
	ListIter nextNode(myStreams);
	NamedNode* node;
	while((node = (NamedNode*)nextNode++) != 0) {
		((CodeStream*)(node->object()))->initialize();
	}
}
