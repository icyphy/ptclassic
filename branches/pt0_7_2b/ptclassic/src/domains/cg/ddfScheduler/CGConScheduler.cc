static const char file_id[] = "CGConScheduler.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGConScheduler.h"
#include "CGTarget.h"
#include "Galaxy.h"
#include "EventHorizon.h"
#include "CGDDFCluster.h"
#include "CGPortHole.h"
#include "Error.h"

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

 Programmer:  Soonhoi Ha

 Methods for CGConScheduler class.
**************************************************************************/

		//////////////////
		//  constructor
		//////////////////

CGConScheduler :: CGConScheduler(MultiTarget* t, ostream* l): 
	mtarget(t), logstrm(l), refGal(0), optNum(1), syncSz(0) {
	LOG_NEW; temp1 = new Profile;
	LOG_NEW; temp2 = new Profile;

	syncSendStars.initialize();
	syncReceiveStars.initialize();
}

CGConScheduler :: ~CGConScheduler() {
	LOG_DEL; delete temp1;
	LOG_DEL; delete temp2;

	syncSendStars.deleteAll();
	syncSendStars.initialize();
	syncReceiveStars.deleteAll();
	syncReceiveStars.initialize();
}

		//////////////////
		//   setup
		//////////////////

void CGConScheduler :: setup() {
	CGMacroClusterGal* gal = (CGMacroClusterGal*) galaxy();

	// initialize galaxy.
        gal->initialize();

	// Topology checking
	// At the same time, the sequential quasi-static schedules are formed.
	if (!examine(gal)) return;

	if (!closerExamine()) return;

	// member initialization
	initMembers();
}

	
		//////////////////////
		//   setProcs
		//////////////////////

void CGConScheduler :: setProcs(int n) {

	numProcs = n;
	temp1->create(n);
	temp2->create(n);
}


		//////////////////////
		//   calcProfile
		//////////////////////

int CGConScheduler :: calcProfile(int resWork, IntArray* pAvail) {

	int flag = FALSE;
	IntArray* avail;
	if (!pAvail) {
		LOG_NEW; avail = new IntArray(numProcs);
		flag = TRUE;
	} else {
		avail = pAvail;
	}
	
        // temp profile initialize
        // If flip = 0, temp1 contains the optimal profile,
        // if flip = 1, temp2 contains the optimal profile.
        int flip = 0;
        temp1->initialize();

        int best = 1;   // Optimal number of the assigned processors.
        setProfile(best, resWork, temp1);
        int idleCost = temp1->totalIdleTime(*avail, numProcs);
        double bestCost = temp1->getTotalCost();
	saveOptInfo();

        // If the residual work is so small that the other
        // processors can not be effectively used, increase the
        // cost of the profile by that extra idle time.
        if (idleCost > resWork) bestCost += idleCost - resWork;

        // find the minimum value of the profile cost and the number
        // of processors at that time.
        Profile* temp;          // profile pointer.
        for (int i = 1; i < numProcs; i++) {
                if (flip == 0)  temp = temp2;
                else            temp = temp1;
                temp->initialize();
                if (!setProfile(i+1, resWork, temp)) continue;
                double bCost = temp->getTotalCost();

                // if the cost is larger than the best cost, go next.
                if (bCost <= bestCost) {
                        int iCost = temp->totalIdleTime(*avail,numProcs)
                                    - resWork;
                        if (iCost > 0) bCost += iCost;
                        if (bCost < bestCost) {
                                best = i + 1;
                                bestCost = bCost;
                                if (!flip) flip = 1;
                                else       flip = 0;
				saveOptInfo();
                        }
                }
        }

        // Now copy the optimal profile.
        if (flip == 0)   temp = temp1;
        else            temp = temp2;
        Profile* store = profile;
        temp->copyIt(store);
        store->setEffP(temp->getEffP());       // store the optimal value N.
        store->setTotalCost(bestCost);

	// return optimal number of assigned processors.
	LOG_DEL; if (flag) delete avail;
	optNum = best;
	return best;
}

		//////////////////////
		//   fixProfile
		//////////////////////

void CGConScheduler :: fixProfile(int nP, int resWork, IntArray* pAvail) {

	int flag = FALSE;
	int n = nP;
	IntArray* avail;
	if (!pAvail) {
		LOG_NEW; avail = new IntArray(numProcs);
		flag = TRUE;
	} else {
		avail = pAvail;
	}
	
        temp1->initialize();

        if (!setProfile(n, resWork, temp1)) {
		Error::abortRun("Specified number is too large. Check ",
		"the number of again.");
		return;
	}
	saveOptInfo();
		
        int idleCost = temp1->totalIdleTime(*avail, numProcs);
        double bestCost = temp1->getTotalCost();

        // If the residual work is so small that the other
        // processors can not be effectively used, increase the
        // cost of the profile by that extra idle time.
        if (idleCost > resWork) bestCost += idleCost - resWork;

        temp1->copyIt(profile);
        profile->setEffP(temp1->getEffP());    // store the optimal value N.
        profile->setTotalCost(bestCost);

	// return optimal number of assigned processors.
	optNum = nP;
	LOG_DEL; if (flag) delete avail;
}

static int _compareType (CGStar& s, const char* t) {
	const char* nm = s.className();
	const char* p = nm + strlen(s.domain());
	int flag = strcmp(p,t);
	if (!flag) return TRUE;
	nm = s.readTypeName();
	return !strcmp(nm,t);
}

// if "nm" is a porthole name, return the outside geodesic of the macro actor
// if "nm" is given "control", return the outside geodesic associated with
//     a Fork star.
// if "nm" is a star name. return the outside geodesic associated with non-
//     control port
// if "nm" is given null, return the outside geodesic assocaited with a sdf
//     star

Geodesic* CGConScheduler :: findGeo(CGStar* macroS, const char* nm) {
	CGClustPortIter nextp(*owner);
	CGClustPort* p = 0;
	while ((p = nextp++) != 0) {
		CGClustPort* inp = p->inPtr();
		CGMacroCluster* ins = inp->parentClust();
		CGAtomCluster* as = 0;
		if (!nm) {
			if (ins->asSpecialBag()) break;
			else continue;
		} 
		as = (CGAtomCluster*) ins;
		if (_compareType(as->real(), nm)) {
			if (strcmp(inp->real().name(), "control")) break;
		} else if (!strcmp(inp->real().name(), nm)) {
			break;
		} else if (!strcmp(nm, "control")) {
			if (_compareType(as->real(), "Fork")) break;
		}
	}
	if (!p) return 0;
	
	BlockPortIter np(*macroS);
	PortHole* realp;
	while ((realp = np++) != 0) {
		if (!strcmp(realp->name(), p->name())) {
			return realp->geo();
		}
	}
	return 0;
}

// if flag = 0, input.
// if flag = 1, output and connected to "Case" or "EndCase" or others
// if flag = 2, output and connected to "Fork" and eventually to "control".
CGPortHole* CGConScheduler :: boundaryPort(CGMacroCluster* s, int flag) {
	CGClustPortIter nextp(*s);
	CGClustPort* p;
	while ((p = nextp++) != 0) {
		if ((!flag && p->isItInput()) || (flag && p->isItOutput())) {
			if (flag) {
			   CGMacroCluster* cs = p->far()->parentClust();
			   if (cs->asSpecialBag()) continue;
			   else {
				CGAtomCluster* as = (CGAtomCluster*) cs;
				if (flag == 1) {
				     if (as->real().isItFork())
					continue;
				} else if (!as->real().isItFork())
					continue;
			   }
			}
			break;
		} 
	}
	if (!p) return 0;

	// now, return the porthole of the copy star on the boundary of
	// the cluster.
	return copyPortHole(p);
}
	
CGPortHole* CGConScheduler :: copyPortHole(CGClustPort* p) {
	CGClustPort* temp = p->inPtr();
	const char* pname = temp->real().name();
	ParNode* smallest = (ParNode*) temp->parentClust()->myMaster();
	DataFlowStar* copyS = smallest->getCopyStar();
	BlockPortIter piter(*copyS);
	PortHole* x;
	while ((x = piter++) != 0) {
		if (!strcmp(x->name(), pname)) return (CGPortHole*) x;
	}
	return 0;
}
	
		//////////////////////
		//   buildSyncLinks
		//////////////////////

// build Synchronization links between processors.
void CGConScheduler :: buildSyncLinks(Profile* pf) {
	// renew star lists
	syncSendStars.deleteAll();
	syncSendStars.initialize();
	syncReceiveStars.deleteAll();
	syncReceiveStars.initialize();

	// obtain the necessary information.
	syncSz = pf->getEffP();

	DataFlowStar* newS;
	DataFlowStar* newR;
	for (int i = 1; i <= pf->numInstance(); i++) {
		int syncP = pf->assignedTo(i,0);
		Target* t = mtarget->child(syncP);
		// create Send and Receive.
		for (int j = 1; j < syncSz; j++) {
			int to = pf->assignedTo(i,j);
			newS = mtarget->createSend(syncP,to,1);
			newR = mtarget->createReceive(syncP,to,1);
			newS->setTarget(t);
			newR->setTarget(mtarget->child(to));
			newS->setNameParent(nameComm(i,j,0), 0);
			newR->setNameParent(nameComm(i,j,1), 0);
			syncSendStars.put(*newS);
			syncReceiveStars.put(*newR);
			mtarget->pairSendReceive(newS, newR);
		}
	}
}
			
const char* CGConScheduler :: nameComm(int invoc, int ix, int flag) {
	StringList name = owner->real()->name();
	name << '_';
	if (flag == 0)      name << "snd0";
	else if (flag == 1) name << "rcv0";
	else if (flag == 2) name << "snd1";
	else if (flag == 3) name << "rcv1";
	else if (flag == 4) name << "snd2";
	else if (flag == 5) name << "rcv2";
	name << '_' << invoc << '_' << ix;
	return hashstring(name);
}

CGStar* CGConScheduler :: syncCommStar(BlockList& b, int inv, 
				       int ix, int flag) {
	const char* nm = nameComm(inv, ix, flag);
	CGStar* s = (CGStar*) b.blockWithName(nm);
	if (!s) {
		StringList out;
		out << "No star name: " << nm << " in the comm. star list";
		Error :: abortRun(out);
	}
	return s;
}

		/////////////////////////////
		//   Common utility methods
		//////////////////////////////

int CGConScheduler :: addSendCodes(Geodesic* gd, int invoc, CGTarget* t) {
	CGStar* s; 
	for (int i = 1; i < syncSz; i++) {
		s = syncCommStar(syncSendStars, invoc, i, 0);
		CGPortHole* p = (CGPortHole*) s->portWithName("input");
		if (!p) {
			Error :: abortRun(*s, "no input porthole");
			return FALSE;
		}
		p->switchGeo(gd);
		if (!t->incrementalAdd(s)) return FALSE;
		p->revertGeo(0);
	}
	return TRUE;
}

		/////////////////////////////
		//   downLoadBodyCode
		//////////////////////////////

int CGConScheduler :: downLoadBodyCode(CGMacroClusterBag* cbag, int pix,
					CGTarget* t) {
	int profileIndex = pix % optNum;
	return cbag->parSched()->getProc(profileIndex)->genCodeTo(t);
}	
	

// virtual functions
int CGConScheduler :: getStatistics() { return FALSE; }
int CGConScheduler :: assumeExecTime() { return 0; }
int CGConScheduler :: examine(CGMacroClusterGal*) { return FALSE; }
int CGConScheduler :: closerExamine() { return FALSE; }
int CGConScheduler :: setProfile(int, int, Profile*) { return 0; }
int CGConScheduler :: downLoadCode(CGStar*, int, int, int) { return FALSE; }

// do-nothing
int CGConScheduler :: run() { return TRUE; }
void CGConScheduler :: setStopTime(double) {}
void CGConScheduler :: initMembers()	{}
void CGConScheduler :: saveOptInfo()	{}
void CGConScheduler :: buildDataSyncs(Profile*)	{}

