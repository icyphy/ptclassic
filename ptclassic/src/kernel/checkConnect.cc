/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 8/6/90

 check a Galaxy to see if it's all connected.  Return a StringList
 of disconnected stars or stars with no ports.

*************************************************************************/
#include "Galaxy.h"
#include "StringList.h"
#include "Block.h"

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
