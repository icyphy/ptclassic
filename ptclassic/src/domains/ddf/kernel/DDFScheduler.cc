#include "type.h"
#include "SDFStar.h"
#include "SDFWormhole.h"
#include "DDFScheduler.h"
#include "DDFWormhole.h"
#include "Output.h"
#include "StringList.h"
#include "FloatState.h"
#include "Geodesic.h"
#include "GalIter.h"

#include "CaseScheduler.h"
#include "ForScheduler.h"
#include "DoWhileScheduler.h"
#include "SDFScheduler.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/9/90
 Revisions: (1/9/91) add dynamic error detecting routines.

These are the methods for the dynamic dataflow scheduler.

**************************************************************************/

extern const char DDFdomainName[];

void fireSource(Star&, int);

// melt the SDFinDDFWormholes in a larger one.
Galaxy* deCrust(Block* dw);

// make a unique portname of an auto-galaxy
const char* galPortName(const char* starName, const char* portName); 

// correct numTokens value for each EventHorizon.
void renewNumTokens(Block* b, int flag);

// expand an auto-galaxy from the "org" block.
static void expandGal (Galaxy& galaxy, Galaxy* g, Block* org); 

// make a connection to the porthole(galP)  of an auto-wormhole
static void reConnect(PortHole* eveP, PortHole* galP); 

// make connection of auto-wormholes
static void connectWormholes (Galaxy&);

// special treat of arcs with delays
static void handleDelays();

// change DDFWormhole to SDFWormhole
Star* changeDDFtoSDF (Star* s) ;
			
static CaseScheduler caseSched;
static ForScheduler  forSched;
static DoWhileScheduler dowhileSched;
static SDFScheduler sdfSched;

/*******************************************************************
 error messages for inconsistent graphs
*******************************************************************/

static char* err0 = " lies in an inconsistent DDF system : \n";
static char* err1_1 = "First check DELAY-FREE LOOP or ";
static char* err2_1 = " needs too large input buffer size (>";
static char* err2_2 = ") \nFirst check INCONSISTENT SAMPLE RATES or ";
static char* err2_3 = "other semantic errors \n... (sorry for poor hints)...";
static char* err2_4 = "\nYou may increase the max buffer size by defining a ";
static char* err2_5 = "FloatState (maxBufferSize) \nin the DDF galaxy.";

#define MAXTOKEN 1024
static int maxToken;
static int lazyDepth;
static int overFlow;
static StringList msg;

/*******************************************************************
		Main DDF scheduler routines
*******************************************************************/

int isSource(const Star& s) {
	BlockPortIter nextp(s);
	PortHole* p;

	while ((p = nextp++) != 0) {
		if (p->isItInput()) {
			int numP = p->myGeodesic->numInitialParticles;
			if (!numP || p->numTokens() > numP)
				return FALSE;
		}
	}
	return TRUE;
}

	////////////////////////////
	// displaySchedule
	////////////////////////////

StringList DDFScheduler :: displaySchedule () {
	return "DDF schedule is computed at run-time\n";
}

extern int warnIfNotConnected (Galaxy&);

	////////////////////////////
	// setup
	////////////////////////////

int DDFScheduler :: setup (Block& b) {
	haltRequestFlag = 0;

	Galaxy& galaxy = b.asGalaxy();

	// if canDom is already set SDF, do SDFScheduling
	if (canDom == SDF) 
		return (sdfSched.setup(galaxy));
		
	GalStarIter nextStar(galaxy);

	// initialize sourceBlocks list
	sourceBlocks.initialize();
	numFiring = 0;

	// check connectivity
	if (warnIfNotConnected (galaxy)) {
		return FALSE;
	}

	// star initialize.  We allow any domain ending in "DF"
	// (for example, SDF, DDF).  Also compute the size.
	Star* s;
	while ((s = nextStar++) != 0) {
		const char* dom = s->domain();
		int l = strlen(dom) - 2;
		if (strcmp(dom + l, "DF") != 0) {
			Error::abortRun (*s, " is not a DDF or SDF star");
			return FALSE;
		}
		// member initialization.
		s->prepareForScheduling();
	}

	galaxy.initialize();

	// If user gives the option of numOverlapped, set it.
	FloatState* ist = (FloatState*) galaxy.stateWithName("numOverlapped");
	if (ist) numOverlapped = int(double(*ist));

	// If user gives the option of schedulePeriod, set it.
	FloatState* fst = (FloatState*) galaxy.stateWithName("schedulePeriod");
	if (fst) schedulePeriod = float ((double) *fst);

	// If user gives the option of maxBufferSize, set it.
	FloatState* bst = (FloatState*) galaxy.stateWithName("maxBufferSize");
	if (bst) maxToken = int(double(*bst));
	else	 maxToken = MAXTOKEN;

	currentTime = schedulePeriod;
	overFlow = FALSE;

	if (restructured == FALSE) {
		// fancy stuff...
		// auto-creation of SDF wormholes, decide the most efficient
		// scheduler (Case, For, DoWhile, Recur)
		makeSDFWormholes(galaxy);
		selectScheduler(galaxy);
		restructured = TRUE;
	}

	if (canDom == DDF) {
		GalStarIter nextS(galaxy);
		Star* sW;

		galSize = 0;
		while ((sW = nextS++) != 0) {
			// put the source block into the sourceBlocks list.
			if (isSource(*sW))
				sourceBlocks.put(sW);
			galSize++;
		}

		if (galaxy.parent() && galaxy.parent()->isItWormhole()) 
			renewNumTokens(galaxy.parent(), FALSE);
	}
			
		
	return !haltRequestFlag;
}


	////////////////////////////
	// run
	////////////////////////////

// Check through the block list of the galaxy. If meet a runnable star,
// run it. Repeat  until there is no runnable star over one complete scan.
// Later, we will compare the performance of list-scan with that of 
// queue-management scheme which requires lots of memory (de/)allocations.
//
// EXPLICIT STOPPING CONDITION : # of firing of all sources - 
// default stopTime = 0
int
DDFScheduler :: run (Block& block) {

	msg.initialize();
	msg += err0;

	if (haltRequestFlag) {
		Error::abortRun(block, ": Can't continue after run-time error");
		return FALSE;
	}

	Galaxy& galaxy = block.asGalaxy();

	switch(canDom) {
		case Case : return(caseSched.run(galaxy));
			 break;
		case For : return(forSched.run(galaxy));
			 break;
		case DoWhile : return(dowhileSched.run(galaxy));
			 break;
		case Recur : return(recurSched.run(galaxy));
			 break;
		case SDF : 
			sdfSched.setStopTime((float) stopTime);
			return(sdfSched.run(galaxy));
			break;
		default : break;
	}
		
	// initialize the SpaceWalk member
	GalStarIter nextStar(galaxy);

	int scanSize = galSize;		// how many not-runnable stars scanned

	while (numFiring < stopTime && !haltRequestFlag) {

	   int numStop;
	   
	   // turn on the option of "overlapped execution"
	   if (numFiring + int(numOverlapped) >= stopTime) 
		numStop = stopTime - numFiring;
	   else		
		numStop = int(numOverlapped);

	   // fire source blocks...
	   numFiring += numStop;
	   ListIter nextb(sourceBlocks);
	   for (int i = sourceBlocks.size(); i > 0; i--) {
		Star& s = *(Star*)nextb++;
		fireSource(s, numStop);
		if (haltRequestFlag) break;
	   }

	   scanSize = galSize;
   
	   while (scanSize) {

		// look at the next star in the block-list
		Star* s = nextStar++;
		if (!s) {
			nextStar.reset();
			s = nextStar++;
		}
		
		// check the star is runnable
		// run it as many times as we can.
		if (isRunnable(*s)) {
			// run the star
			do {
				s->fire();
				if (haltRequestFlag) exit(1);
			} while (isRunnable(*s));

			// reset scanSize
			scanSize = galSize;
		} else {
			scanSize--;
		}
		// some geodesic is full 
		if (overFlow)  { 
			msg += err2_1;
			msg += maxToken;
			msg += err2_2;
			msg += err2_3;
			msg += err2_4;
			msg += err2_5;
			Error :: abortRun(*s,msg);
			return FALSE;
		}
		// there is a deadlock condition.
		if (lazyDepth > galSize) {
			msg += err1_1;
			msg += err2_3;
			Error :: abortRun(*s,msg);
			return FALSE;
		}

	   } /* inside while */

	   currentTime += schedulePeriod;

	} /* outside while */
	return TRUE;
}

int DDFScheduler :: isRunnable(Star& s) {

	// if source, return FALSE
	if (isSource(s)) return FALSE;

	lazyDepth = 1;		 // initialize lazyDepth

	// check the domain of the star
	// if DDFStar...

	if (strcmp (s.domain(), DDFdomainName) == 0) {

		// if user specified waitPort exists...
		DDFStar* p = (DDFStar*) &s;
		if (p->hasWaitPort()) {
		
			// check Geodesic overflow

			BlockPortIter nextp(s);
			for (int i = s.numberPorts(); i > 0; i--) {
				// look at the next port
				PortHole& tp = *nextp++;
				if (tp.isItInput()) {
					// check Geodesic size 
					int geoSize = tp.numTokens();
					if (geoSize > maxToken) {
						overFlow = TRUE;
						msg += tp.readFullName();
						return FALSE;
					}
				}
			}

			// regular routine

			int req = p->reqTokens();
			while (req > 0) {
			   // if # tokens are static, return FALSE.
			   // if on wormhole boundary, return FALSE.
			   DDFPortHole* po = (DDFPortHole*) p->waitPort;
			   if (po->isDynamic() && po->far()->isItOutput()) {
				// if dynamic, enable lazy evaluation.
				lazyDepth++;
				if (!lazyEval((Star*) po->far()->parent()))
					return FALSE;
				lazyDepth--;
			   } else
					return FALSE;
			   req = p->reqTokens();
			}
			return TRUE;
		}
	}

	// if all input ports have no data, return FALSE
	int count = 0;
	BlockPortIter nextp(s);
	PortHole *p;
	while ((p = nextp++) != 0) {
		if (p->isItInput() && 
			p->numTokens() > p->myGeodesic->numInitialParticles)
			count++;
	}
	if (count == 0)		
		return FALSE;

	// if SDFStar, or no-specified waitPort for DDFStar,....

	return (checkLazyEval(&s));
}
	

int DDFScheduler :: lazyEval(Star* s) {

	if (lazyDepth > galSize) return FALSE;

	if (strcmp (s->domain(), DDFdomainName) == 0) {

		// if user specified waitPort exists...
		DDFStar* p = (DDFStar*) s;
		if (p->hasWaitPort()) {

			int req = p->reqTokens();
			DDFPortHole* po = (DDFPortHole*) p->waitPort;
			while (req > 0) {
			   // if on wormhole boundary, return FALSE.
			   if (po->far()->isItOutput()) {
				lazyDepth++;
				if (!lazyEval((Star*) po->far()->parent())) {
					return FALSE;
				}
				lazyDepth--;
			   } else {
					return FALSE;
			   }
			   req = p->reqTokens();
			}
			s->fire();
			return TRUE;
		}
	}

	int result = checkLazyEval(s);
	if (result) {
		// fire the star
		s->fire();
		return TRUE;
	} else
		return FALSE;
}
	
int DDFScheduler :: checkLazyEval(Star* s) {
	BlockPortIter nextp(*s);

	for (int i = s->numberPorts(); i > 0; i--) {
		
		// look at the next port
		PortHole& p = *nextp++;
		if (p.isItInput()) {

			int geoSize = p.numTokens();

			int req = p.numberTokens - geoSize;
			if ((req > 0) && 
				// check wormhole, recursive star
				(p.far()->isItInput())) {
				 return FALSE;
			}

			Star* str = (Star*) p.far()->parent();
			if (strcmp (str->domain(), DDFdomainName) == 0) {
			   // check if wormhole...
			   if (str->mySched() == (Scheduler*) this) {
				// check dynamic port for ddf star.
			   	DDFPortHole* po = (DDFPortHole*) p.far();
			   	if (req > 0 && po->isDynamic()) {
					return FALSE;
				}
			   }
			}

			while (req > 0) {
				lazyDepth++;
				if (!lazyEval(str)) 
					return FALSE;
				lazyDepth--;
				req = p.numberTokens - p.numTokens();
			}

			// check Geodesic size if it exceeds the limit
			if (geoSize > maxToken) {
				msg += p.readFullName();
				overFlow = TRUE;
				return FALSE;
			}
		}
	}

	return TRUE;
}

void fireSource(Star& s, int k) {
			
	// check how many unused tokens are on output arcs.
	int min = 10000;		// large enough number
	int minIn = 1000;
	BlockPortIter nextp(s);
	for (int j = s.numberPorts(); j > 0; j--) {
		PortHole& port = *nextp++;
		Geodesic* g = port.myGeodesic;

	  	if (port.numberTokens == 0) {
			Error::abortRun(s,
			      ": output port has undefined number of tokens");
			return;
		}
		if (port.isItOutput()) {
			int r = (g->size())/port.numberTokens;
			if (r < min) min = r;
		} else {
			int k = port.numTokens() / port.numberTokens;
			if (minIn > k) minIn = k;
		}
			
	}

	if (s.numberPorts() == 0) min = 0;

	// fire sources "k-min" times.
	if (minIn > k - min) minIn = k - min;
	for (int i = 0; i < minIn; i++) 
		s.fire();
}

// my domain
const char* DDFScheduler :: domain () const { return DDFdomainName ;}

// variable used for generating names
int DDFScheduler::nW = 0;

// generate a new name for Wormholes of SDF domain
const char* DDFScheduler::wormName() {
	char buf[16];
	sprintf (buf, "!worm%d", ++nW);
	return savestring (buf);
}

// name a porthole of an auto-galaxy with a combination of the star name
// and a portname.
const char* galPortName(const char* starName, const char* portName) {
	char buf[32];
	int len = strlen(starName);
	strcpy(buf, starName);
	buf[len++] = '(';
	strcpy(buf+len, portName); 
	len += strlen(portName);
	strcpy(buf+len, ")"); 
	return savestring (buf);
}

// SequentialList for the Geodesic with delays
SequentialList delayGeos;

	////////////////////////////
	// makeSDFWormholes
	////////////////////////////

// In the DDF domain, we group SDF stars and wormholes of SDF domain
// as a as large a DDFWormhole of SDF domain as possible.
// This grouping will improve the runtime speed by utilizing the
// compile-time SDF scheduler.
// Also, with modified topology, check whether it is a special domain
// or not : Case, For, DoWhile, Recur.

void DDFScheduler :: makeSDFWormholes (Galaxy& galaxy) {

	GalStarIter nextStar(galaxy);

	sdfWorms.initialize();
	delayGeos.initialize();
	canDom = UnKnown;
	int flagDDF = 0;

	Star* current = nextStar++;
	while (current != NULL) {

	    Star* s = current;
	    Star* prev = current;
 
	    // detect a SDF star which is not grouped into a newly
	    // created SDF galaxy.  If found, create a new SDF galaxy (newG)
	    // and add the SDF star into the galaxy.
	    // Then, expand the galaxy as much as possible.
	
	    int branch = 0;
	    if (s->isItWormhole()) {
		const char* dom = s->mySched()->domain();
		if (!strcmp(dom, "SDF")) branch = 2;
		else if (!strcmp(dom, "DDF")) {
			DDFScheduler* sr = (DDFScheduler*) s->mySched();
			int temp = sr->isSDFType();
			if (temp == 1) {
				branch = 1;
				s = changeDDFtoSDF(current);
			} else if (temp == 2) branch = 2;
		}
	    } else {
		if (strcmp(s->domain(), DDFdomainName)) branch = 1;
	    }

	    if (branch) {

		Galaxy* newG = new Galaxy;
		sdfWorms.put(newG);
		newG->setBlock(wormName(), 0);
		newG->myDomain = savestring("SDF");

		if (branch == 2) {
			// take off wormhole crust
		    Galaxy* inG = deCrust(s);
		    newG->addBlock(*inG, inG->readName());
		    expandGal(galaxy, newG, inG);
		} else {
			newG->addBlock(*s, s->readName());
			expandGal(galaxy, newG, s);
		}
		current = nextStar++;
		if (current == prev) current = NULL;
		galaxy.removeBlock(*prev);

	    } else {

		current = nextStar++;
		flagDDF++;
	    // if it is a DDF Star or a Wormhole of non-SDF domain, 
	    // do nothing.  And check the candidate domain.

		if (s->isItWormhole())  canDom = DDF;

		// check the candidate domain...

		if (canDom != DDF) {
		DDFStar* ds = (DDFStar*) s;
		const char* nm = ds->readClassName();

			// Only Recur domain may have "Self" star.
		if (!strcmp(nm, "Self")) {
			if (canDom == UnKnown || canDom == EndCase) 
				canDom = Recur;
			else if (canDom != Recur) canDom = DDF;

			// Only For domain may have "UpSample" star.
		} else if ((!strcmp(nm, "UpSample")) 
			|| (!strcmp(nm, "DownSample"))) {
			if (canDom == UnKnown) canDom = For;
			else if (canDom != For) canDom = DDF;

			// Case and DoWhile domain may have "Case" star
		} else if (!strcmp(nm, "Case")) {
			BlockPortIter np(*s);
			PortHole* p;

			while((p = np++) != 0) {
			    if (p->isItInput() == p->far()->isItInput()) {
				if (p->isItInput()) {
				    if (canDom == UnKnown || canDom == EndCase) 
					canDom = Case;
				    else if (canDom != Case) canDom = DDF;
				} else {
				    if (canDom == UnKnown) canDom = DoWhile;
				    else if(canDom != DoWhile) canDom = DDF;
				}
			    } 
			}   // end of inner while
		} else if (!strcmp(nm, "EndCase")) {
			BlockPortIter np(*s);
			PortHole* p;

			while((p = np++) != 0) {
			    if (p->isItInput() == p->far()->isItInput()) {
				if (p->isItOutput()) {
				  if (canDom == UnKnown) 
					canDom = EndCase;
				  else if((canDom != Case)&&(canDom != Recur))
					canDom = DDF;
				} 
			    } 
			}   // end of inner while
		} else {
			canDom = DDF;
		}} 	// end of inner if
	    }	// end of outermost if
	}	// end of while

	if (canDom == EndCase)	canDom = Case;
	else if ((canDom == UnKnown) && (!flagDDF)) canDom = SDF;

	// Handle  the arcs of delays.
	// If the delay is between two different wormholes, create
	// wormhole portholes for the arc. Otherwise ignore it.
	handleDelays();

	ListIter nextb(sdfWorms);

	// if canDom = SDF, do not make wormholes.
	if (canDom == SDF) {
		for (int i = sdfWorms.size(); i > 0; i--) {
			Galaxy* inG = (Galaxy*) nextb++;
			galaxy.addBlock(*inG, inG->readName());
		}
		galaxy.myDomain = savestring("SDF");
		return;
	}

	// Now we have SDF galaxies. Make them as DDF Wormholes.
	for (int i = sdfWorms.size(); i > 0; i--) {
		Galaxy* inG = (Galaxy*) nextb++;
		DDFWormhole* dW = new DDFWormhole(*inG);
		dW->prepareForScheduling();
		galaxy.addBlock(*dW, inG->readName());
		dW->start();
		renewNumTokens(dW, TRUE);
	} 

	// make connections for broken arcs.
	connectWormholes(galaxy);
}

	////////////////////////////
	// selectScheduler
	////////////////////////////

Scheduler* DDFScheduler :: selectScheduler(Galaxy& galaxy) {

	// select a candidate domain
	int flag = 1;
	Scheduler* realSched;
	switch(canDom) {
		case Case : flag = caseSched.setup(galaxy);
			 break;
		case For : flag = forSched.setup(galaxy);
			 break;
		case DoWhile : flag = dowhileSched.setup(galaxy);
			 break;
		case Recur : flag = recurSched.setup(galaxy);
			 break;
		case SDF : flag = sdfSched.setup(galaxy);
		        if (galaxy.parent()) 
				renewNumTokens(galaxy.parent(), TRUE);
			break;
		default : flag = 0;
			  break;
	}

	if (!flag)
		canDom = DDF;
}
			
	////////////////////////////
	// connectWormholes
	////////////////////////////

// During the procedure of making DDF Wormholes of SDF domain, we
// broke the connections at the boundary of the Wormholes.
// This routine remakes the connections.

void connectWormholes (Galaxy& g) {

	if (g.numberPorts()) {

	    BlockPortIter galps(g);
	    // verify the wormhole boundary connection.
	    Block* parW = g.parent();

	    // exclude DDF Self in the recursion construct
	    if (parW->isItWormhole()) {
		BlockPortIter nextp(*parW);
		PortHole* p;

		while((p = nextp++) != 0) {
			EventHorizon* eveP = ((EventHorizon*) p)->ghostPort;
			PortHole* galP = eveP->far();
			// check the connection
			if (eveP != galP->far()) {
				reConnect(eveP, galP);	
				// change alias of the galaxy
				PortHole* gp;
				galps.reset();
				while ((gp = galps++) != 0) {
					if (galP == &(gp->realPort())) {
						gp->setAlias(*(eveP->far()));
						break;	
					}
				}
			}
		}
	    }
	}

	// verify the connections of the DDF Stars and Wormholes.
	// Call reConnect if necessary.

	GalStarIter nextStar(g);
	Star* s;

	while ((s = nextStar++) != 0) {

	   if ((!s->isItWormhole()) ||
			strcmp(s->mySched()->domain(), "SDF")) {

		BlockPortIter nextp(*s);
		PortHole* p;

		while((p = nextp++) != 0) {
			PortHole* galP = p->far();
			// check the connection
			if ( galP && (p != galP->far())) 
				reConnect(p, galP);	
		}
	    }
	}

	// Now, make connections around each arc with delays if
	// the arc connects two different auto-wormholes.
	ListIter nextGeo(delayGeos);
	Geodesic* geo;

	while ((geo = (Geodesic*) nextGeo++) != 0) {
		PortHole* src = geo->sourcePort();
		PortHole* dest = geo->destPort();
		// if delay between SDF stars or DDFWormhole of SDF domain
		if ((src->isItInput() == src->far()->isItInput()) &&
			(dest->isItInput() == dest->far()->isItInput())) {
			EventHorizon* eS = 
				((EventHorizon*) src->far())->ghostPort;
			EventHorizon* dS = 
				((EventHorizon*) dest->far())->ghostPort;
			// reuse the old geodesic
			geo->disconnect(*src);
			geo->disconnect(*dest);
			geo->setSourcePort(*eS, geo->numInitialParticles);
			geo->setDestPort(*dS);
			eS->initialize();
			dS->initialize();
		}
	}
}

	////////////////////////////
	// reConnect
	////////////////////////////

// When create a new DDFWormhole of SDF domain, the connections around
// the Wormholes are all broken.  galP points to the PortHole inside the
// Wormhole and eveP points to the PortHole of the other block.
// Now, connect eveP to the proper EventHorizon of the Wormhole, which by
// turns connected to galP.

void reConnect(PortHole* eveP, PortHole* galP) {

	EventHorizon* realP = ((EventHorizon*) galP->far())->ghostPort;
	int delay = eveP->myGeodesic->numInitialParticles;
	eveP->myGeodesic->disconnect(*galP);
	eveP->disconnect(1);
	if (galP->isItInput())
		eveP->connect(*realP, delay);
	else
		realP->connect(*eveP, delay);
	realP->initialize();
}
		
	////////////////////////////
	// expandGal
	////////////////////////////

// Once create a SDF galaxy, expand it as much as possible.
// First, include any adjacent SDF stars into the galaxy.
// Second, if there is any Wormholes of SDF domain, remove the
// crust of Wormhole and make the inside galaxy as a normal
// galaxy.

void expandGal (Galaxy& galaxy, Galaxy* g, Block* org) {

	BlockPortIter nextp(*org);
	PortHole* p;

	while((p = nextp++) != 0) {
		PortHole& realP = p->newConnection();
		Star* s = (Star*) realP.far()->parent();;

		// do not uncover a galaxy if the galaxy is included
		// in a auto-galaxy already.
		int touched = 0;
		Block* sB = s->parent();
		while (sB && !sB->isItWormhole()) {
			if (sB == (Block*) g) { touched = 1;
					        break; }
			else sB = sB->parent();
		}

	   if (!touched) {

		// check which branch the star is in.
	    	int branch = 0;
	    	if (s->isItWormhole()) {
			const char* dom = s->mySched()->domain();
			if (realP.isItInput() != realP.far()->isItInput()) {
				if (!strcmp(dom, "SDF")) branch = 2;
				else if (!strcmp(dom, "DDF")) {
					DDFScheduler* sr = 
					(DDFScheduler*) s->mySched();
					int temp = sr->isSDFType();
					if (temp == 1) {
						branch = 1;
						s = changeDDFtoSDF(s);
					} else if (temp == 2) branch = 2;
				}
			}
	    	} else {
			if (strcmp(s->domain(), DDFdomainName)) branch = 1;
	    	}

		int initNum = realP.myGeodesic->numInitialParticles;
		if (branch == 1) {
			// if SDF star, put it into the galaxy and
			// call expandGal for this star.
		   if (!initNum) {
			g->addBlock(*s, s->readName());
			galaxy.removeBlock(*s);
			expandGal(galaxy, g, s);
		   } else {
			// put the geodesic only once in case there is a delay
			// on the arc
			if (realP.isItInput())
				delayGeos.put(realP.myGeodesic);
		   }
		} else if (!branch) {
			// if DDF star, DDFWormhole of non-SDF domain,
			// or wormhole boundary, 
			// create a galaxy porthole and alias it.
			if (realP.isItInput()) {
				InPortHole* gp = new InPortHole;
				g->addPort(gp->setPort(
			   galPortName(org->readName(), realP.readName()), g));
				gp->setAlias(realP);
			} else {
				OutPortHole* gp = new OutPortHole;
				g->addPort(gp->setPort(
			   galPortName(org->readName(), realP.readName()), g));
				gp->setAlias(realP);
			}
		} else {
		   if (!initNum) {
			// DDF wormhole of SDF domain
			// First, take off wormhole crust
			Galaxy* inG = deCrust(s);
			g->addBlock(*inG, inG->readName());
			galaxy.removeBlock(*s);
			expandGal(galaxy, g, inG);
		   } else {
			// put the geodesic only once in case there is a delay
			// on the arc
			if (realP.isItInput())
				delayGeos.put(realP.myGeodesic);
		   }
		}
	    }
	}
}
			 
	////////////////////////////
	// handleDelays
	////////////////////////////

void handleDelays() {
	
	ListIter nextGeo(delayGeos);
	Geodesic* geo;

	while ((geo = (Geodesic *) nextGeo++) != 0) {
		PortHole* src = geo->sourcePort();
		PortHole* dest = geo->destPort();

		Block* sB = src->parent()->parent();
		while(sB && strncmp(sB->readName(), "!worm", 5))
			sB = sB->parent();

		Block* dB = dest->parent()->parent();
		while(dB && strncmp(dB->readName(), "!worm", 5))
			dB = dB->parent();

		// compare galaxies
		// if the geodesic will connect two different wormholes,
		// create  galaxy portholes.
		if (sB != dB) {
			// if different create galaxy ports.
			InPortHole* ip = new InPortHole;
			dB->addPort(ip->setPort( 
				galPortName(dest->parent()->readName(), 
				dest->readName()), dB));
			ip->setAlias(*dest);
	
			OutPortHole* op = new OutPortHole;
			sB->addPort(op->setPort(
			   	galPortName(src->parent()->readName(), 
				src->readName()), sB));
			op->setAlias(*src);
		}
	}
}

	////////////////////////////
	// deCrust
	////////////////////////////

Galaxy* deCrust(Block* dw) {

	BlockPortIter nextp(*dw);
	PortHole* p;
	Galaxy* myGal;

	while((p = nextp++) != 0) {
		int delay = p->myGeodesic->numInitialParticles;
		PortHole* tempP = p->far();
		EventHorizon* eveP = (EventHorizon*) p;
		PortHole* galP = eveP->ghostPort->far();
		Block* galS = galP->parent();
		while (!galS->parent()->isItWormhole())
			galS = galS->parent();
		myGal = (Galaxy*) galS;
		tempP->disconnect(1);
		galP->disconnect(1);
		delete eveP->ghostPort;
		delete eveP;
		if (tempP->isItInput()) 
			galP->connect(*tempP, delay);
		else
			tempP->connect(*galP, delay);
	}

	return myGal;
}

	////////////////////////////
	// renewNumTokens
	////////////////////////////

// after compile-time scheduling, change the numberTokens parameter
// of the EventHorizons.

void renewNumTokens (Block* b, int flag) {

	BlockPortIter nextp(*b);
	PortHole* p;

	while ((p = nextp++) != 0) {
		EventHorizon* eveP = (EventHorizon*) p;
		EventHorizon* ghostP = (EventHorizon*) eveP->ghostPort;
		PortHole* bP = ghostP->far();
		int num = bP->numberTokens;
		Star* s = (Star*) bP->parent();
		// if b is a SDF wormhole
		if (flag == TRUE) {
			SDFStar* sS = (SDFStar*) s;
			num *= sS->repetitions.numerator;
			p->numberTokens = num;
			ghostP->numberTokens = num;
			p->setMaxDelay(0);
			ghostP->setMaxDelay(0);
			if (p->far()) p->initialize();
		} else {	// DDF wormhole
			if (num == 0) num = 1;
			p->numberTokens = num;
			ghostP->numberTokens = num;
			p->initialize();
		}
	}
}

	////////////////////////////
	// changeDDFtoSDF
	////////////////////////////

// if the DDF domain is a predefined construct, change the crust
// from DDFWormhole to SDFWormhole. ->necessary for restructuring.

Star* changeDDFtoSDF (Star* s) {

	SequentialList ports;

	ports.initialize();
	
	// save far() ports of the EventHorizons.
	BlockPortIter nextp(*s);
	PortHole* p;

	while ((p = nextp++) != 0) {
		ports.put(p->far());
	}
	
	// deCrust the DDFWormhole
	Galaxy* inGal = deCrust(s);

	// make SDFWormhole
	SDFWormhole* sW = new SDFWormhole(*inGal);

	// remake connections.
	ListIter farp(ports);

	while ((p = (PortHole*) farp++) != NULL) 
		reConnect(p, p->far());
		
	renewNumTokens(sW, FALSE);
	return sW;
}
