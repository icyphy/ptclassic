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

StringList checkConnect (Galaxy& g) {
	StringList msg;

	for (int n = g.numberBlocks(); n > 0; n--) {
		Block& b = g.nextBlock();
		if (!b.isItAtomic()) {
			msg += checkConnect (b.asGalaxy());
		}
		else {
			int nPorts = b.numberPorts();
			if (nPorts == 0) {
				msg += b.readFullName();
				msg += " has no portholes\n";
			}
			else for (int m = nPorts; m > 0; m--) {
				PortHole& p = b.nextPort ();
				if (!p.far()) {
					msg += b.readFullName();
					msg += " is not connected\n";
				}
			}
		}
	}
	return msg;
}

int warnIfNotConnected (Galaxy& g) {
	StringList msg = checkConnect (g);
	if (msg.size() > 0) {
		errorHandler.error (msg);
		return 1;
	}
	else return 0;
}

