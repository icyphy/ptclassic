static const char file_id[] = "checkConnect.cc";
/**************************************************************************
Version identification:
$Id$

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

StringList checkConnect (Galaxy& g) {
	GalStarIter next(g);
	StringList msg;

	Star* s;
	// iterate over every star
	while ((s = next++) != 0) {
		// error if no portholes
		if (s->numberPorts() == 0) {
			msg += s->fullName();
			msg += " has no portholes\n";
			Error::mark(*s);
			break;
		}
		// error if any porthole is disconnected
		BlockPortIter nextp(*s);
		PortHole* p;
		while ((p = nextp++) != 0) {
			if (!p->far()) {
				msg += p->fullName();
				msg += " is not connected\n";
				Error::mark(*p->parent());
			}
		}
	}
	return msg;
}

int warnIfNotConnected (Galaxy& g) {
	StringList msg = checkConnect (g);
	const char* str = msg;	// cast to char*
	// check for non-zero-length message
	if (str && *str) {
		Error::abortRun (str);
		return 1;
	}
	else return 0;
}

