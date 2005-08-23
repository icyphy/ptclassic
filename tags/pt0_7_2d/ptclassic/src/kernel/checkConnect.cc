static const char file_id[] = "checkConnect.cc";

/**************************************************************************
Version identification:
@(#)checkConnect.cc	2.12	01/29/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer:  J. T. Buck
 Date of creation: 8/6/90

 checkConnect checks a Galaxy to see if it is completely connected.
 The function returns a StringList containing all the names
 of disconnected stars or stars with no ports.

 warnIfNotConnected prints the result of checkConnect out with
 Error::error if if contains any messages; it returns 1 if an
 error has occurred, otherwise 0.

*************************************************************************/
#include "Galaxy.h"
#include "StringList.h"
#include "Block.h"
#include "Error.h"
#include "GalIter.h"
#include "checkConnect.h"

// The maximum number of 'not connected' errors we will display
// If this number is too large, then we won't see the message
// at the end about the total number of errors, as the
// error messages get truncated in the log.
#define MAX_NOT_CONNECTED_ERRORS 10

// checkConnect checks a Galaxy to see if it is completely connected.
StringList checkConnect(Galaxy& g) {
	GalStarIter next(g);
	StringList msg;
	int errorCount = 0;
	// iterate over every star to make sure each is connected
	Star* s;
	while ((s = next++) != 0) {
		// error if any porthole is disconnected
		// ignore the case when the star has no portholes, as
		// this is used in some domains like IPUS
		BlockPortIter nextp(*s);
		PortHole* p;
		while ((p = nextp++) != 0) {
			if (!p->far()) {
			        if (errorCount++ < MAX_NOT_CONNECTED_ERRORS) {
				    msg += p->fullName();
				    msg += " is not connected\n";
				    Error::mark(*p->parent());
				} 
			}
		}
	}
	if (errorCount > MAX_NOT_CONNECTED_ERRORS) {
	  msg << "Saw " << errorCount << " `not connected' errors, "
	    << "but only marked the first " << MAX_NOT_CONNECTED_ERRORS
	    << " errors.";
	}
	return msg;
}

// warnIfNotConnected prints result of checkConnect if any ports are
// not conencted
int warnIfNotConnected(Galaxy& g) {
	StringList msg = checkConnect(g);
	int notConnectedFlag = msg.length();	     // non-zero string length
	if (notConnectedFlag) Error::abortRun(msg);
	return notConnectedFlag;
}
