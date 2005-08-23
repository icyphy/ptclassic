/**************************************************************************
Version identification:
@(#)BDFScheduler.cc	2.15	12/08/97

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
 Date of creation: 1/8/91

 BDF Scheduler methods

**************************************************************************/

static const char file_id[] = "BDFScheduler.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "BDFScheduler.h"
#include "BDFStar.h"
#include "BDFSchList.h"
#include "FloatState.h"
#include "Geodesic.h"
#include "GalIter.h"
#include "ConstIters.h"

extern const char BDFdomainName[];

/************************************************************************

	Boolean Synchronous dataflow (BDF) Scheduler

*************************************************************************/

extern const char SDFdomainName[];

#include <stream.h>

/////////////////////////////////////////////////////////////////////////
// class to simulate a geodesic
// We put tokens on these geodesics corresponding to TRUE, FALSE, and
// UNKNOWN.
/////////////////////////////////////////////////////////////////////////

const int UNKNOWN = 2;

class SimGeo {
private:
	int sz;
	ParticleStack pstack;
public:
	SimGeo(int v,int n) : sz(0), pstack(0) { nput(v,n);}
	int size() const { return sz;}
	void put(int);
	int get();
	int nextValue();
	void nput(int,int);
	int nget(int);
	static IntParticle proto;
};

IntParticle SimGeo::proto;

void SimGeo::put(int newv) {
	// make a new particle, put the value into it.
	Particle* p = proto.clone();
	*p << newv;
	// stick it into the "back"
	pstack.putTail(p); sz++;
}

void SimGeo::nput(int newv, int nPart) {
	if (newv < 0 || newv > 2) {
		cout << "Hey! Bad nput value: " << newv << "!\n";
		newv = UNKNOWN;
	}
	for (int i = 0; i < nPart; i++)
		put (newv);
}

int SimGeo::get() {
	if (sz <= 0) {
		Error::abortRun ("Internal error in SimGeo::get: empty!");
		return -1;
	}
	Particle* p = pstack.get();
	sz--;
	int value = int(*p);
	p->die();
	if (value < 0 || value > 2) {
		cout << "SimGeo::get: bad value " << value << "!\n";
		value = UNKNOWN;
	}
	return value;
}

// return value at head of queue, without removing
// if nothing in queue, we return UNKNOWN (this is not an error:
// it simply means we do not know the boolean value of the next
// particle)
int SimGeo::nextValue() {
	if (sz == 0) return UNKNOWN;
	Particle* p = pstack.get();
	int value = int(*p);
	pstack.put(p);		// push back onto front
	return value;
}


int SimGeo::nget(int nPart) {
	int res = UNKNOWN;
	for (int i = 0; i < nPart; i++) res = get();
	return res;
}

/*******************************************************************
		Main BDF scheduler routines
*******************************************************************/


	////////////////////////////
	// runOnce
	////////////////////////////

// Run a single iteration of the schedule.  On the first execution,
// do the preamble as well (if it exists)
void BDFScheduler :: runOnce () {
	if (inPreamble) {
		BlockListIter next(preamble);
		Block* s;
		while ((s = next++) != 0) {
			s->run();
			if (haltRequested()) return;
		}
		inPreamble = FALSE;
	}
// Run the regular part.
	mySchedule->run(*this);
}

int BDFScheduler::prepareGalaxy() {
	if (! galaxy()) return FALSE;

	// initialize galaxy and all contents.
	galaxy()->initialize();
	galSize = setStarIndices(*galaxy());
	nPorts = setPortIndices(*galaxy());
	// turn on debugging if there is a state named debug
	if (galaxy()->stateWithName("debug")) debug = TRUE;
	return TRUE;
}

// fn to check if a star obeys BDF semantics.
static int isBDF(DataFlowStar& star) {
	if (star.isSDF()) return TRUE;
	// check any varying ports to see if they
	// obey BDF rules
	DFStarPortIter nextPort(star);
	DFPortHole* p;
	while ((p = nextPort++) != 0) {
		if (p->isDynamic()) {
			int rel = p->assocRelation();
			if (rel != BDF_TRUE && rel != BDF_FALSE) {
				Error::abortRun(star,
						" not a valid BDF actor");
				return FALSE;
			}
		}
	}
	return TRUE;
}

int BDFScheduler::checkStars() {
// Allocate space for the SchedInfo structures.  Check the stars
// for BDF-ness or SDF-ness, and set up the structures.
	LOG_DEL; delete [] starInfo;
	LOG_DEL; delete [] portInfo;
	LOG_NEW; starInfo = new BDFStarSchedInfo[galSize];
	LOG_NEW; portInfo = new BDFPortSchedInfo[nPorts];

	GalStarIter nextStar(*galaxy());
	Star* s;
	// initialize scheduling information for all stars.
	// make sure they are of the right domain.

	while ((s = nextStar++) != 0) {
		if (!s->isA("DataFlowStar")) {
			Error::abortRun(*s, " is not a DataFlowStar");
			invalid = TRUE;
			return FALSE;
		}
		DataFlowStar& star = *(DataFlowStar*)s;
		if (!isBDF(star)) {
			invalid = TRUE;
			return FALSE;
		}
		// if we get here it is OK.
		initInfo (star);
	}
	return TRUE;
}

int BDFScheduler::computeSchedule(Galaxy& galaxy) {
        int i;
	GalStarIter nextStar(galaxy);
	Star* s;

	// debug -- print repetition counts
	if (debug) {
		cout << "Repetition counts:\n";
		nextStar.reset();
		while ((s = nextStar++) != 0) {
			cout << s->fullName() << ": " <<
				info(*s).reps << "\n";
		}
	}

	if (invalid) return FALSE; // detect inconsistency

	// check if preamble needed.  Mark which stars write boolean
	// values that are used -- we cannot fire those stars in the
	// preamble.
	for (i = 0; i < galSize; i++) starInfo[i].writesBoolean = FALSE;

	int preambleNeeded = FALSE;

	BDFPortSchedInfo* pi = portInfo;
	for (i = 0; i < nPorts; i++, pi++) {
		if (pi->relDelay) preambleNeeded = TRUE;
		if (pi->finalAssoc) {
			BDFStarSchedInfo& sinfo =
				info(pi->finalAssoc->parent()->asStar());
			sinfo.writesBoolean = TRUE;
		}
	}

	preamble.initialize();
	LOG_DEL; delete mySchedule;
	LOG_NEW; mySchedule = new BDFSchedule;
	if (preambleNeeded) {
		if (debug) cout << "Generating preamble\n";
		inPreamble = TRUE;
	}
	else inPreamble = FALSE;

	// MAIN LOOP
	// After a scheduler pass, the passValue tells us whether the
	// scheduler is finished.  After the pass, the meaning of the
	// possible values is:
	// 	2 - scheduler is finished
	// 	1 - scheduler is deadlocked
	// 	0 - more passes are required

	// if we have a preamble we have three stages: generate the
	// preamble; complete the schedule, and compensate for the
	// preamble (a "postamble").

	int preambleCompensate = FALSE;
	do {
		passValue = 2;
		deferredStar = 0;
		int starFiredOnThisPass = FALSE;

		nextStar.reset();
		while ((s = nextStar++) != 0) {
			int runResult;

			do {
				runResult = addIfWeCan(*s,deferredFiring);
				if (runResult == 0)
					starFiredOnThisPass = TRUE;
			} while (runResult == 0);
		}
		
		// If the deferred firing option is set, we must now schedule
		// the deferred star.  We only do this if nothing else fired.

		if (deferredStar && !starFiredOnThisPass)
			addIfWeCan (*deferredStar);

		// completed this pass.  Check for preamble magic.

		if (passValue == 1 && inPreamble) {
			// once deadlock is hit, end preamble
			inPreamble = FALSE;
			if (debug) cout << "Preamble completed\n";
			passValue = 0;
			preambleCompensate = TRUE;
		}
		else if (passValue != 0 && preambleCompensate) {
			// completed or deadlocked: compensate for preamble now
			if (debug) cout << "Compensating for preamble\n";
			subtractOffPreamble();
			passValue = 0;
			preambleCompensate = FALSE;
		}
	} while (passValue == 0);
	// END OF MAIN LOOP
	
	if (passValue == 1) {
		Error::abortRun (*dead,
				 ": DEADLOCK in a loop containing this block");
		invalid = TRUE;
	}

// Flag that we need to do the preamble first.
	inPreamble = preamble.size() ? TRUE : FALSE;
	return !invalid;
}

// subtract off executions corresponding to the preamble.
void BDFScheduler::subtractOffPreamble() {
	BlockListIter next(preamble);
	Block* b;
	while ((b = next++) != 0) {
		Star& s = b->asStar();
		info(s).noTimes -= 1;
	}
}

// This routine simulates running the star, adding it to the
// schedule where it can be run

int BDFScheduler::addIfWeCan (Star& star, int defer) {

	int runRes = simRunStar(star,defer);
	if (runRes == 0) {
		const BoolTerm& reps = info(star).reps.num();
		if (inPreamble)
			// no conditions on preamble execution
			preamble.put(star);
		else
			mySchedule->add(star,reps,info(star).numToksConst,
					info(star).writesBoolean);
		passValue = 0;
	}
	else if ((runRes == 1 || runRes == 3) && passValue != 0) {
		passValue = 1;
		dead = &star;
	}
	return runRes;
}

/*******************************************************************
		Routines for computing the repetitions
*******************************************************************/

// WARNING: For disconnected systems, the repetitions properties
// are all multiplied by the least common multiple of the denominators
// of *all* the repetitions properties, not just those is a connected
// subsystem.  Hence for disconnected systems, the relative number
// of times the subsystems are invoked may not be obvious.

	////////////////////////////
	// repetitions
	////////////////////////////

int BDFScheduler :: repetitions () {

	// Initialize the least common multiple, which after all the
	// repetitions properties have been set, will equal the lcm
	// of the all the denominators.
	lcm = 1;

	// The following iteration occurs exactly as many times as
	// there are blocks in the universe.
	// Note that for a connected universe, this iteration would
	// entirely unnecessary.  Is there some way to get rid of it?
	// Should we allow disconnected universes?
	GalStarIter next(*galaxy());
	Star* s;
	while ((s = next++) != 0) {
		// First check to see whether a repetitions property has
		// been set.  If so, do nothing.  Otherwise, compute the
		// repetitions property.
		if(!info(*s).set) {
			// Set repetitions to 1 and set repetitions for
			// all blocks connected to the current block.
			info(*s).reps = 1;
			info(*s).set = TRUE;
			reptConnectedSubgraph(*s);
			if (invalid) return FALSE;
		}
	}

	// Once all the repetitions properties are set, the lcm member
	// contains the least common multiple of all the denominators
	// of all the fractional repetitions.  To convert them to
	// integers, we multiply through by lcm.

	for (int idx = 0; idx < galSize; idx++) {
		starInfo[idx].reps *= lcm;
		starInfo[idx].reps.simplify();
	}
	return TRUE;
}


	////////////////////////////
	// reptConnectedSubgraph
	////////////////////////////

// The following function sets the repetitions property of
// all atomic blocks connected to block, directly or indirectly.
// It is assumed that block has its repetitions property already set.

int BDFScheduler :: reptConnectedSubgraph (Block& block) {

	// Step through each portHole
	BlockPortIter nextp(block);
	for(int i = block.numberPorts(); i>0; i--) {
		PortHole& nearPort = *nextp++;
		// ignore if the port is at the boundary of the wormhole.
		if (nearPort.atBoundary()) continue;

		PortHole& farPort = *(nearPort.far());
		// recursively call this function on the farSideBlock,
		// having determined that it has not previously been done.

		if(reptArc(nearPort,farPort))
			reptConnectedSubgraph (*(farPort.parent()));
	}
	return TRUE;
}

BoolTerm
BDFScheduler :: numTok (const PortHole& p) {
	BDFPortSchedInfo& pi = info(p);
	if (pi.finalAssoc == 0) {
		// no associated boolean
		return BoolTerm(pi.num);
	}
	else return BoolTerm(pi.num,BoolSignal(*pi.finalAssoc,pi.neg));
}

	////////////////////////////
	// reptArc
	////////////////////////////

// This function sets the repetitions property of two blocks
// on opposite sides of a connection.  It is assumed that nearPort
// belongs to a block that has already had its repetitions property set.
// If in addition farPort has had its repetitions property set,
// then consistency is checked, and FALSE is returned.  Otherwise,
// TRUE is returned.

int BDFScheduler :: reptArc (PortHole& nearPort, PortHole& farPort){
	BDFStarSchedInfo& nearStarInfo = info(nearPort.parent()->asStar());
	BDFStarSchedInfo& farStarInfo = info(farPort.parent()->asStar());
	BoolFraction& nearReps = nearStarInfo.reps;
	BoolFraction& farReps = farStarInfo.reps;
	BoolFraction farShouldBe;

	if (debug)
		cout << "reptArc: nearPort = " << nearPort.fullName()
		     <<  ", nearReps = " << nearReps
		     << "; farPort = " << farPort.fullName()
		     << "\n nearNumTok = " << numTok(nearPort)
		     <<  ", farNumTok = " << numTok(farPort) << "\n";

	// compute what the far star repetitions property should be.
	farShouldBe = nearReps * BoolFraction(numTok(nearPort),numTok(farPort));
	farShouldBe.simplify();

	if (!farStarInfo.set) {
		// farStarRepetitions has not been set, so set it
		farReps = farShouldBe;
		farStarInfo.set = TRUE;
		// update the least common multiple of the denominators
		lcm.lcm(farShouldBe.den());
		if (debug) {
			cout << "farReps set to " << farReps;
			cout <<  ", lcm = " << lcm << "\n";
		}
		return TRUE;
	}
	else {
		// farReps has been set, so test for equality.  If we get
		// an error, reduce mismatch to lowest terms.
		if (!(farReps == farShouldBe)) {
			BoolFraction ratio(farReps);
			ratio /= farShouldBe;
			ratio.simplify();
			StringList msg = "Sample rate problem between ";
			msg += nearPort.parent()->fullName();
			msg += " and ";
			msg += farPort.parent()->fullName();
			msg += ": ";
			msg += ratio.num();
			msg += " != ";
			msg += ratio.den();
			Error::abortRun(msg);
			invalid = TRUE;
		}
		return FALSE;
	}
}


/*******************************************************************
		Routines for computing the schedule
*******************************************************************/

	////////////////////////////
	// computeTokens
	////////////////////////////
// compute the number of tokens we must move on each arc, given
// the pending boolean values (if any)
// if useKnownBools is false, we do not use our knowledge and
// act as if the values are unknown.

void BDFScheduler :: computeTokens(const Star& star, int useKnownBools) {
	CBlockPortIter nextp(star);
	const PortHole* port;
	while ((port = nextp++) != 0) {
		BDFPortSchedInfo& pinfo = info(*port);
		int v = UNKNOWN;
		if (useKnownBools && pinfo.assoc) {
			v = info(*pinfo.assoc).geo->nextValue();
			if (pinfo.relation == FALSE && v != UNKNOWN)
				v = !v;
		}
		pinfo.nmove = v ? pinfo.num : 0;
	}
}


	////////////////////////////
	// simRunStar
	////////////////////////////

static char chtok[] = "FTU";	// debug

int BDFScheduler :: simRunStar (Star& atom, int deferFiring) {

	int test = notRunnable(atom);
	if(test) return test;	// return if the star cannot be run

	// If we get to this point without returning, then the star can be run.

	PortHole* port;

	// An important optimization for code generation:
	// Postpone any execution of a star feeding data to another
	// star that is runnable.  Also postpone if each output Geodesic
	// has enough data on it to satisfy destination stars.
	// This is optional because it slows down the scheduling.

	if(deferFiring && deferIfWeCan(atom))
		return 1;

// Determine how many tokens we require to read or write.
	computeTokens (atom);

// Read all input values.  This simplified code will only handle
// related booleans if a single value is read!

	if (debug) cout << "Running " << atom.name() << ":\n";
	BlockPortIter nextp(atom);
	while ((port = nextp++) != 0) {
		BDFPortSchedInfo& pinfo = info(*port);
		if (port->isItOutput() || port->far()->isItInput())
			continue;
		pinfo.lastRead = pinfo.geo->nget (pinfo.nmove);

		if (debug) cout << port->name() << "(" << pinfo.nmove
			<< ")-> " << chtok[pinfo.lastRead] << " ";
	}

// finally, write all output values.

	nextp.reset();
	while ((port = nextp++) != 0) {
		BDFPortSchedInfo& pinfo = info(*port);
		if (port->isItInput() || port->far()->isItOutput())
			continue;
		int vwrite = valueToWrite(*port);
		pinfo.geo->nput (vwrite, pinfo.nmove);

		if (debug) cout << chtok[vwrite] << "(" << pinfo.nmove <<
			")-> " << port->name() << " ";
	}

	if (debug) cout << "\n";

	// Increment noTimes
	info(atom).noTimes += 1;

	// Indicate that the block was successfully run
	return 0;
}

	///////////////////////////
	// displaySchedule
	///////////////////////////

StringList BDFScheduler :: displaySchedule() {
	StringList out;
	out << "{\n  { scheduler \"Buck's BDF scheduler\" }\n";
	if (preamble.size()) {
		out << "  { preamble {\n";
		BlockListIter next(preamble);
		Block* b;
		while ((b = next++) != 0) {
			out << "    { fire " << b->fullName() << " }\n";
		}
		out << "  } }\n";
	}
	out << mySchedule->print();
	out << "}\n";
	return out;
}


	///////////////////////////
	// valueToWrite
	///////////////////////////

// we write a token with value TRUE, FALSE, or UNKNOWN, based
// on the associated porthole.  We write UNKNOWN whenever we
// have no information to the contrary.

int BDFScheduler :: valueToWrite (const PortHole& port) {
	BDFPortSchedInfo& pinfo = info(port);
	if (pinfo.assoc == 0 ||
	    (pinfo.relation != BDF_SAME) && (pinfo.relation != BDF_COMPLEMENT))
		return UNKNOWN;
	BDFPortSchedInfo& srcinfo = info(*pinfo.assoc);
	int srcval = srcinfo.geo->nextValue();
	if (srcval == UNKNOWN) return UNKNOWN;
	if (srcval != TRUE && srcval != FALSE) {
		StringList nm = port.fullName();
		StringList anm = pinfo.assoc->fullName();
		cout << "port: " << nm << " assoc: " << anm <<
				" bad srcval: " << srcval << "!\n";
		return UNKNOWN;
	}
	return pinfo.relation == BDF_SAME ? srcval : !srcval;
}

	///////////////////////////
	// deferIfWeCan
	///////////////////////////

// return TRUE if we defer atom, FALSE if we do not.
// Postpone any execution of a star feeding data to another
// star that is runnable.  Also postpone if each output Geodesic
// has enough data on it to satisfy destination stars.

int BDFScheduler :: deferIfWeCan (Star& atom) {

	int i;
	PortHole* port;
	BlockPortIter nextp(atom);
	// Check to see whether any destination blocks are runnable
	for(i = atom.numberPorts(); i>0; i--) {
		// Look at the next port in the list
		port = nextp++;

		// cannot be deferred if on the boundary of wormhole.
		if ((port->isItOutput()) && (port->isItOutput() == port->far()->isItOutput()))
			return FALSE;

		Star& dest = port->far()->parent()->asStar();

		// If not an output, or destination is not
		// runnable, skip to the next porthole

		if(!port->isItOutput() || notRunnable(dest,FALSE) != 0)
			continue;
		
		// An output block is runnable.  To minimize buffering,
		// we do not want to fire atom yet.  We mark the first
		// deferred star.
		if (!deferredStar)
			deferredStar = &atom;

		// Give up on this star -- it has been deferred
		return TRUE;
	}

	// Alternatively, the block might be deferred if its output
	// Geodesics all have enough data to satisfy destination
	// stars, even if the destination stars are not runnable.

	// We must detect the case that there are no output ports
	int outputPorts = FALSE;
	nextp.reset();	// restart iterator
	for(i = atom.numberPorts(); i>0; i--) {

		// Look at the farSidePort of the next port in the list
		port = nextp++;
		// Skip if it is not output or not connected
		if (!port->isItOutput() || port->far() == NULL)
			continue;
		
		port = (port->far());
		BDFPortSchedInfo& pinfo = info(*port);

		// The farside port is an input.  Check Particle supply
		// if not enough, atom cannot be deferred.
		if(pinfo.geo->size() < pinfo.num)
			return FALSE;
			
		// Since the farside port is an input, the nearside is
		// an output
		outputPorts = TRUE;
	}

	// If we get here, all destinations have enough data,
	// and the block can be deferred, assuming there are output ports.
	if (outputPorts && !deferredStar)
		deferredStar = &atom;
	return outputPorts;
}

	////////////////////////////
	// notRunnable
	////////////////////////////

// Return 0 if the star is can be run now,
// 1 if it can't be run now, but should be runnable later (it has not
// reached its designated number of iterations), and 2 if it
// has been run as often as it needs to be, or 3 if we are in the
// preamble and it writes a token that is used as a Boolean.
// It is assumed that the denominator of the
// repetitions member is unity (as it should be).
// When useKnownBools is false, we ignore knowledge of known booleans.
// This is used when testing for deferring stars since we don't have
// the required tokens yet on the simulated geodesics.

const char* nrstatus[] = { "RUNNABLE", "NOT READY", "COMPLETED",
"WRITES BOOLEAN: Not allowed in preamble" };

int BDFScheduler :: notRunnable (Star& atom, int useKnownBools) {
	StringList atomName = atom.fullName();
	if (debug) cout << "notRunnable(" << atomName << "): ";
	int i, v = 0;
	BlockPortIter nextp(atom);
	BDFStarSchedInfo& sinfo = info(atom);
	// preamble check
	if (inPreamble && sinfo.writesBoolean)
		v = 3;
	// Check to see whether the requisite repetitions have been met.
	else if (sinfo.reps.num().constTerm <= sinfo.noTimes)
		v = 2;

	// Step through all the input ports, checking to see whether
	// there is enough data on the inputs
	else {
		computeTokens (atom, useKnownBools);
		for(i = atom.numberPorts(); i>0; i--) {

			// Look at the next port in the list
			PortHole& port = *nextp++;
			BDFPortSchedInfo& pinfo = info(port);

			if(port.isItInput() && port.far()->isItOutput()) {
				// If not in the interface of Wormhole
				if(pinfo.geo->size() < pinfo.nmove) {
					// not enough data
					v = 1;
					break;
				}
			}
		}
	}
	if (debug) cout << nrstatus[v] << "\n";
	return v;
}

// function to allow wormholes to determine timing.
int BDFScheduler::reps (Star& s) {
	return info(s).reps.num().constTerm;
}

// constructor for BDFStarSchedInfo
BDFStarSchedInfo::BDFStarSchedInfo() : numToksConst(TRUE), set(0),
		noTimes(0), writesBoolean(0) {}

BDFPortSchedInfo::BDFPortSchedInfo() : num(0), assoc(0), relation(0),
		neg(0), finalAssoc(0), relDelay(0), geo(0) {}

// function to initialize information for a DataFlowStar
// In this function and the next, it is assumed that the info structure
// has the values set in the constructor.

void BDFScheduler::initInfo(DataFlowStar& star) {
	DFStarPortIter nextPort(star);
	BDFStarSchedInfo& sinfo = info(star);
	DFPortHole* p;
	while ((p = nextPort++) != 0) {
		commonPortInfo(*p);
		BDFPortSchedInfo& pinfo = info(*p);
		pinfo.assoc = p->assocPort();
		pinfo.relation = p->assocRelation();
		if (conditional(*p))
			sinfo.numToksConst = FALSE;
		pinfo.traceBack (*p);
	}
}

// initialize the parts that work for any type of porthole.
void BDFScheduler::commonPortInfo(DFPortHole& port) {
	BDFPortSchedInfo& pinfo = info(port);
	pinfo.num = port.numXfer();
	pinfo.inFlag = port.isItInput();
	BDFPortSchedInfo& farInfo = info(*(port.far()));
	// create the simulated geodesic, or attach to the existing one.
	if (farInfo.geo) pinfo.geo = farInfo.geo;
	else {
		LOG_NEW; pinfo.geo = new SimGeo (FALSE,port.numTokens());
	}
}

// this function traces all the assocPort()s and relations back
// to find the origin of a signal.  We count the delays we cross,
// if any.
void BDFPortSchedInfo::traceBack (DFPortHole& port) {
	relDelay = 0;
	if (!conditional(port)) {
		finalAssoc = 0;
		neg = 0;
		return;
	}
// trace back the signal
	neg = (port.assocRelation() == BDF_FALSE);
	DFPortHole* where = (DFPortHole*)port.assocPort();
// see if there is an earlier source for the signal.  Repeat as follows:
// for inputs, replace with the output that produced the signal.
// for outputs with BDF_SAME or BDF_COMPLEMENT relationships, replace
// with corresponding input (e.g. find the fork input).
	while (1) {
		if (where->isItInput()) {
			if (where->atBoundary()) break;
			relDelay += where->numTokens();
			where = (DFPortHole*)where->far();
		}
		else if (SorC((BDFRelation)where->assocRelation())) {
			// seek an input that is the same or complement.
			// If none found, use original.
			DFPortHole* mark = where;
			while (!where->isItInput()) {
				if (where->assocRelation() == BDF_COMPLEMENT)
					neg = !neg;
				where = (DFPortHole*)where->assocPort();
				if (where == mark) {
					finalAssoc = where;
					return;
				}
			}
		}
		else break;
	}
// we are now back to the original source.
	finalAssoc = where;
}

BDFPortSchedInfo::~BDFPortSchedInfo() {
	if (inFlag) {LOG_DEL; delete geo;}
}

BDFScheduler::~BDFScheduler() {
	LOG_DEL; delete mySchedule;
	LOG_DEL; delete [] starInfo;
	LOG_DEL; delete [] portInfo;
}

// static debug flag
int BDFScheduler::debug = FALSE;

// Functions used in executing the schedule

void BDFScheduler::saveBooleans(Star& s) {
	BlockPortIter next(s);
	PortHole *p;
	while ((p = next++) != 0) {
		if (p->isItOutput()) {
			Particle* bv = p->geo()->get();
			info(*p).lastRead = int(*bv);
			p->geo()->pushBack(bv);
		}
	}
}

