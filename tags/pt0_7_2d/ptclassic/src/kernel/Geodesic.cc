static const char file_id[] = "Geodesic.cc";
/****************************************************************
Version identification:
@(#)Geodesic.cc	2.20	11/10/97

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

 Programmer: J. T. Buck
 (original version of Geodesic was in Connect, by E. A. Lee and
  D. G. Messerschmitt -- this one is quite different)

Geodesic

Geodesic is where Particles reside in transit between
Blocks. It is a container class, which stores and retreives
Particles. Actually it stores only Particle*s, so that its
operation is independent of the type of Particle.

Geodesics can be created named or unnamed.

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Geodesic.h"
#include "Block.h"
#include "PortHole.h"
#include "Plasma.h"
#include "StringList.h"
#include "PtGate.h"
#include "IntState.h"

// constructor
Geodesic::Geodesic() : originatingPort(0), destinationPort(0),
maxBufLength(0), gate(0), pstack(0), sz(0), numInitialParticles(0), 
initValues(0)
{ }

StringList Geodesic :: print (int) const {
	StringList out;
	if (isItPersistent()) out += "Persistent ";
	out += "Geodesic: ";
	out += fullName();
	out += "\n";
	if (originatingPort) {
		out += "originatingPort: ";
		out += originatingPort->fullName();
		out += "\n";
	}
	if (destinationPort) {
		out += "destinationPort: ";
		out += destinationPort->fullName();
		out += "\n";
	}
	if (!originatingPort && !destinationPort)
		out += "Not connected.\n";
	return out;
}

int Geodesic :: isItPersistent () const {
	return FALSE;
}

// this function "wires" the connection between portholes.
// it requires friend access to class PortHole.
void Geodesic :: portHoleConnect () {
	if (originatingPort && destinationPort) {
		originatingPort->farSidePort = destinationPort;
		destinationPort->farSidePort = originatingPort;
		originatingPort->myGeodesic = this;
		destinationPort->myGeodesic = this;
	}
}

// this function changes the sourceport.
// 3/2/94 changed to add initDelayValues
PortHole* Geodesic :: setSourcePort (GenericPort& sp, int numDelays,
				     const char* initDelayValues) {
	originatingPort = &sp.newConnection();
	portHoleConnect();
	numInitialParticles = numDelays;
	// Keep a copy of the initDelayValues string, if nonzero,
	// in case the original copy disappears.
	initValues = initDelayValues?hashstring(initDelayValues):0;
	return originatingPort;
}

// this function adjusts the delay.
// 3/2/94 changed to add initDelayValues
void Geodesic :: setDelay (int numDelays, const char* initDelayValues) {
	numInitialParticles = numDelays;
	// Keep a copy of the initDelayValues string, if nonzero,
	// in case the original copy disappears.
	initValues = initDelayValues?hashstring(initDelayValues):0;
}

PortHole* Geodesic :: setDestPort (GenericPort& dp) {
	destinationPort = &dp.newConnection();
	portHoleConnect();
	return destinationPort;
}

int Geodesic :: disconnect (PortHole& p) {
	// It is possible that originatingPort and destinationPort
	// point to the same object; be sure to clear both if so.
	int result = FALSE;
	if (originatingPort == &p) {
		originatingPort = 0;
		result = TRUE;
	}
	if (destinationPort == &p) {
		destinationPort = 0;
		result = TRUE;
	}
	return result;
}

void Geodesic :: initialize()
{
	// Remove any Particles residing on the Geodesic,
	// and put them in Plasma
	pstack.freeup();

	// Initialize the buffer if initDelays has values.
	// Note: need to check for non-empty strings, empty strings
	// are returned from the oct database.
        if(initValues && *initValues != 0 && *initValues != '*') { 
          // get one particle of the right type to generate the rest
          Particle* p = originatingPort->myPlasma->get();
    
          // call particle specific function to generate the other initial
          // particles and automatically put them on the pstack.
          numInitialParticles = p->initParticleStack(parent(),pstack,
                                                     originatingPort->myPlasma,
                                                     initValues);

          // finally, put this particle at the head of the pstack
          if(numInitialParticles > 0)
            pstack.put(p);
          else originatingPort->myPlasma->put(p);

          sz = maxBufLength = numInitialParticles;
        }
        else {
	  if(initValues && *initValues == '*') {
	    // '*' is a marker put in front of old style delays
	    // evaluate the string as an IntState
	    IntState tstate;
	    tstate.setState("initDelays",parent(),initValues+1);
	    tstate.initialize();
	    numInitialParticles = int(tstate);
	  }
          // old code: initialize the buffer to the number of Particles
          // specified in the connection; note that these Particles
          // are initialized by Plasma
          for(int i=numInitialParticles; i>0; i--) {
            Particle* p = originatingPort->myPlasma->get();
            pstack.putTail(p);
          }
          sz = maxBufLength = numInitialParticles;
	}
}

// Similar to initialize() but still keep its infrastructure.
void Geodesic::resetBufferValues() {
	// Remove any Particles residing on the Geodesic,
	// and put them in Plasma
	pstack.freeup();

	// Initialize the buffer if initDelays has values.
	// Note: need to check for non-empty strings, empty strings
	// are returned from the oct database.
        if(initValues && *initValues != 0 && *initValues != '*') { 
          // get one particle of the right type to generate the rest
          Particle* p = originatingPort->myPlasma->get();
    
          // call particle specific function to generate the other initial
          // particles and automatically put them on the pstack.
          numInitialParticles = p->initParticleStack(parent(),pstack,
                                                     originatingPort->myPlasma,
                                                     initValues);

          // finally, put this particle at the head of the pstack
          if(numInitialParticles > 0)
            pstack.put(p);
          else originatingPort->myPlasma->put(p);

        } else {
          // old code: initialize the buffer to the number of Particles
          // specified in the connection; note that these Particles
          // are initialized by Plasma
          for(int i=numInitialParticles; i>0; i--) {
            Particle* p = originatingPort->myPlasma->get();
            pstack.putTail(p);
          }

	}
}

// Functions for determining maximum buffer size during a simulated run.
void Geodesic :: incCount(int n) {
	CriticalSection region(gate);
	sz += n;
	if (sz > maxBufLength) maxBufLength = sz;
}

void Geodesic :: decCount(int n) {
	CriticalSection region(gate);
	sz -= n;
}

void Geodesic :: setMaxArcCount(int n) {
	CriticalSection region(gate);
	if (n > maxBufLength) maxBufLength = n;
}

// fully general get, put, etc functions
Particle* Geodesic :: slowGet() {
	CriticalSection region(gate);
	if (sz > 0) { 
		sz--; return pstack.get();
	}
	else return 0;
}

void Geodesic :: slowPut(Particle* p) {
	CriticalSection region(gate);
	pstack.putTail(p); sz++;
}

//  Push a Particle back into the Geodesic
void Geodesic :: pushBack(Particle* p) {
	CriticalSection region(gate);
	pstack.put(p); sz++;
}

// access head and tail of queue
Particle* Geodesic :: head() const { 
	CriticalSection region(gate);
	return pstack.head();
}

Particle* Geodesic :: tail() const {
	CriticalSection region(gate);
	return pstack.tail();
}


// locking functions
void Geodesic :: makeLock(const PtGate& master) {
	LOG_DEL; delete gate;
	gate = master.makeNew();
}

void Geodesic :: delLock() {
	LOG_DEL; delete gate;
	gate = 0;
}

// destructor
Geodesic :: ~Geodesic () {
	// free all particles (return them to Plasma)
	pstack.freeup();
	delLock();
	// disconnect any connected portholes.  0 argument says
	// not to have PortHole::disconnect try to do things to geodesic
	if (originatingPort) originatingPort->disconnect(0);
	if (destinationPort) destinationPort->disconnect(0);
}

// isa
ISA_FUNC(Geodesic,NamedObj);

// Return the initial delay values string.
const char* Geodesic::initDelayValues() {
  return initValues;
}
