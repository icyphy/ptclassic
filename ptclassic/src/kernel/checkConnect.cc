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

 warnIfNotConnected prints the result of checkConnect out onto
 errorHandler if if contains any messages; it returns 1 if an
 error has occurred, otherwise 0.

*************************************************************************/
#include "Galaxy.h"
#include "StringList.h"
#include "Block.h"
#include "Output.h"
#include "GalIter.h"

StringList checkConnect (Galaxy& g) {
	GalStarIter next(g);
	StringList msg;

	Star* s;
	// iterate over every star
	while ((s = next++) != 0) {
		// error if no portholes
		if (s->numberPorts() == 0) {
			msg += s->readFullName();
			msg += " has no portholes\n";
			break;
		}
		// error if any porthole is disconnected
		BlockPortIter nextp(*s);
		PortHole* p;
		while ((p = nextp++) != 0) {
			if (!p->far()) {
				msg += p->readFullName();
				msg += " is not connected\n";
			}
		}
	}
	return msg;
}

int warnIfNotConnected (Galaxy& g) {
	StringList msg = checkConnect (g);
	char* str = msg;	// cast to char*
	// check for non-zero-length message
	if (str) {
		errorHandler.error (str);
		return 1;
	}
	else return 0;
}

