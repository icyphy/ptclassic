static const char file_id[] = "checkConnect.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

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
		Error::error (str);
		return 1;
	}
	else return 0;
}

