static const char file_id[] = "Nebula.cc";
/******************************************************************
Version identification:
 $Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmers: J. L. Pino & T. M. Parks
 Date of creation: 6/10/94

 A Nebula is an executable Star, a sort of light-weight Wormhole.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Star.h"
#include "Galaxy.h"
#include "Nebula.h"
#include "SimControl.h"
#include "Scheduler.h"
#include "Geodesic.h"
#include "Plasma.h"

extern int setPortIndices(Galaxy&);

// Constructors.
Nebula::Nebula(Star& self, Block* m) : selfStar(self) {
	if (master->isItAtomic()) {
		master = m;
		// Add the star's ports to the internal galaxy,
		// but do not change their parents.
		BlockPortIter starPorts(*master);
		PortHole* port;
		while ((port = starPorts++) != NULL)
		{
			if (port->far()->parent() == (Block*)&master)
				// Exclude self-loop connections.
				continue;
			
			else if (port->atBoundary())
				// Exclude wormhole connections.
				continue;
	       
			else {
				PortHole* clonedPort = clonePort(port);
				selfStar.addPort(*clonedPort);
				clonedPort->
					setNameParent(port.name(),&selfStar);
			}
		}
	}
	else {
		Galaxy* g = (Galaxy*) m;
		int nports = setPortIndices(*g);
		master = NULL;
		LOG_NEW; PortHole** newPorts = new PortHole*[nports];
		for (int i = 0; i < nports; i++)
			newPorts[i] = 0;
		GalStarIter nextStar(*g);
		Star* s;
		while ((s = nextStar++) != 0) {
			Nebula* c = newNebula(s);
			gal.addBlock(c->selfStar,c->selfStar.name());
			BlockPortIter nextPort(c->selfStar);
			PortHole *p;
			while ((p = nextPort++) != 0) {
				newPorts[((NebulaPort*)p)->real().index()]=p;
			}
		}
		// now connect up the Nebula ports to match the real ports.
		// There may be fewer Nebula  ports than real ports if there
		// are self-loops, for such cases, ptable[i] will be null.
		for (i = 0; i < nports; i++) {
			PortHole* out = newPorts[i];
			PortHole& outReal = ((NebulaPort*)out)->real();
			if (!out || out->isItInput()) continue;
			PortHole* in = newPorts[outReal.far()->index()];
			int numDelays = outReal.numInitDelays();
			const char* initDelays = outReal.initDelayValues();
			out->connect(*in,numDelays,initDelays);
			out->geo()->initialize();
		}
		LOG_DEL; delete newPorts;
	}
}

int Nebula::run() {
	if (master)
		return master->run();
	else if (sched)
		return sched->run();
	else
		return FALSE;
}
	
int Nebula::generateSchedule() {
	if (master) {
		return TRUE;
	}
	else {
		NebulaIter nebula(*this);
		Nebula* n;
		while ((n = nebula++) !=0)
			n->generateSchedule();
		sched->setup();
		return ! SimControl::haltRequested();
	}
}
	
void Nebula::absorb(Nebula* victum) {
};

NebulaPort::NebulaPort(PortHole& self, const PortHole& master,const Nebula* parent)
:selfPort(self),pPort(master) {
	selfPort.setPort(real().name(),&parent->star(),INT);
	selfPort.myPlasma = Plasma::getPlasma(INT);
	selfPort.numberTokens = real().numXfer();
	selfPort.indexValue = real().index();
}



