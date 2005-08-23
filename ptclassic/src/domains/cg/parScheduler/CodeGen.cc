static const char file_id[] = "CodeGen.cc";

/*****************************************************************
Version identification:
@(#)CodeGen.cc	1.45	09/10/99

Copyright (c) 1990-1999 The Regents of the University of California.
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

Programmer: Soonhoi Ha
Date of last revision: 09/10/99

special routines to generate the sub universes.
*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "UniProcessor.h"
#include "ParProcessors.h"
#include "KnownBlock.h"
#include "SDFPortHole.h"
#include "Geodesic.h"
#include "ConstIters.h"
#include "dataType.h"
#include "Wormhole.h"
#include <string.h>
#include <stdio.h>              // sprintf()

#include <iostream.h>

PortHole* clonedPort(DataFlowStar* s, PortHole* p) {
	ParNode* n = (ParNode*) s->myMaster();
 	DataFlowStar* copyS = n->getCopyStar();
	return copyS->portWithName(p->name());
}

			//////////////////////
			// sub-Galaxy creation
			//////////////////////

void UniProcessor :: createSubGal() {

	// create data structure
	LOG_DEL; delete subGal;
	LOG_NEW; subGal = new DynamicGalaxy;
	subGal->setNameParent(targetPtr->name(), 0);
	targetPtr->setGalaxy(*subGal);

	// maintain the list of the SDF stars which we have considered
	SequentialList touchedStars;
	touchedStars.initialize();

	// maintain the earliest invocation of a star assigned.
	SequentialList assignedFirstInvocs;
	assignedFirstInvocs.initialize();
	
	static const char* domForClone;

	// create SDF stars.
	ProcessorIter pIter(*this);
	ParNode* n;
	while ((n = pIter.nextNode()) != 0) {
		// the second condition checks whether it is a macro node (or
		// parallel star) node or not.
		if (n->getType() || (n->getProcId() != myId())) continue;
		
		// for each node assigned to the processor
		// we deal with the node of the smallest invocation first.
		DataFlowStar* org = n->myMaster();

		// If already dealt with, continue.
		if (touchedStars.member(org)) continue;
		// We believe temporal locality
		touchedStars.prepend(org);

		ParNode* smallest = (ParNode*) org->myMaster();
		while (smallest && (smallest->getProcId() != myId()))
			smallest = (ParNode*) smallest->getNextInvoc();

		DataFlowStar* copyS;
		// FIXME - the moveStar code assumes that each star is
		// mapped to only one processor.
		if (parent->moveStars) {
		    copyS = n->myMaster();
		}
		else {
		    copyS = n->copyStar(target(), myId(), 1);
		}
		domForClone = copyS->domain();
		if (SimControl::haltRequested()) return;

		subGal->addBlock(*copyS, copyS->name());
		copyS->setTarget(targetPtr);


		ParNode* prevN = 0;
		smallest->setCopyStar(copyS, prevN);
		prevN = smallest;

		copyS->setMaster(smallest);
		assignedFirstInvocs.put(smallest);
		// Set the pointer of the invocations to the cloned star.
		while ((smallest = (ParNode*) smallest->getNextInvoc()) != 0) 
			if (smallest->getProcId() == myId()) {
				smallest->setCopyStar(copyS, prevN);
				prevN = smallest;
			}
	}

	// make connections. 
	// Create "Spread" star or "Collect" star if necessary.
	ListIter siter(assignedFirstInvocs);
	ParNode* pn;
	while ((pn = (ParNode*) siter++) != 0) {
		DataFlowStar* org = pn->myMaster();
		BlockPortIter piter(*org);
		PortHole* p;
// CGDDF currently depends on being able to handle disconnected graphs,
// which is a change from the other parallel schedulers.  The other
// parallel schedulers assume that we have a fully connected graph
// by the time we get here.
// This functionality should probably be inside the cgddf scheduler
// If you want to turn on this functionality, compile this file with:
// -DCGDDF_CODEGEN_CC_WORKAROUND
//  cd $PTOLEMY/obj.$PTARCH/domains/cg/parScheduler; rm -f CodeGen.o
//  make USERFLAGS=-DCGDDF_CODEGEN_CC_WORKAROUND install
//
#ifdef CGDDF_CODEGEN_CC_WORKAROUND
                Block* black = 0;
                Block* cst = 0;
#endif
		while ((p = piter++) != 0) {
#ifdef CGDDF_CODEGEN_CC_WORKAROUND
				// Take care of unconnected portholes

				// Boundary
                        if (!p->far()) {
                                // make a dummy connection to prevent
                                // error message when code generation
                                if (p->isItOutput()) {
                                   if (!black) {
                                      black = KnownBlock::clone("BlackHole",
                                           domForClone);
                                      if (!black) {
                                        Error::abortRun("we need BlackHole",
                                        " star for dummy connection.");
                                        return;
                                      }
                                   }
                                   PortHole* destP =
                                        black->portWithName("input");
                                   clonedPort(org,p)->connect(*destP, 0);
                                } else {
                                   cst = KnownBlock::clone("Const",
                                        domForClone);
                                   if (!cst) {
                                        Error::abortRun("we need Const",
                                        " star for dummy connection.");
                                        return;
                                   }
                                   PortHole* srcP =
                                        cst->portWithName("output");
                                   srcP->connect(*(clonedPort(org,p)), 0);
                                }

                                // TRY IT
                                continue;
                        }
#endif // CGDDF_CODEGEN_CC_WORKAROUND
			DataFlowStar* farS =(DataFlowStar*) p->far()->parent();
			ParNode* farN = (ParNode*) farS->myMaster();
			ParNode* myN = (ParNode*) org->myMaster();
			
			// depending on OSOPreq()
			if (OSOPreq() || (myN->isOSOP() && farN->isOSOP())) {
				makeOSOPConnect(p, org, farS, touchedStars);
			} else {
				makeGenConnect(p, pn, org, farS, touchedStars);
			}
			if (SimControl::haltRequested()) return;
		}
	}
}

			////////////////////////////
			// makeOSOPConnect
			////////////////////////////

void UniProcessor :: makeOSOPConnect(PortHole* p, DataFlowStar* org, 
	DataFlowStar* farS, SequentialList& touchedStars) {

    if (touchedStars.member(farS)) {
	// if we are moving stars, they are already connected
	if (parent->moveStars) return;
	
	// make connection if it is output
	if (p->isItOutput()) {
	    PortHole* destP = clonedPort(farS, p->far());
	    clonedPort(org, p)->connect(*destP, p->numTokens());
	}
    } else {
	ParNode* n = (ParNode*) farS->myMaster();
	PortHole* cp = parent->moveStars?p:clonedPort(org, p);
	if (p->isItInput()) {
	    makeReceive(n->getProcId(),cp,p->numTokens(),0,p);
	}
	else {
	    makeSend(n->getProcId(), cp, 0,p);
	}
    }
}

			///////////////////////////////
			// makeGenConnect
			///////////////////////////////

void UniProcessor :: makeGenConnect(PortHole* p, ParNode* pn, DataFlowStar*,
			  DataFlowStar* farS, SequentialList& touchedStars) {

	ParNode* farN = 0;
	if (touchedStars.member(farS)) {
		farN = (ParNode*) farS->myMaster();
		while (farN->getProcId() != myId())
			farN = (ParNode*) farN->getNextInvoc();
	}
	if (p->isItInput()) {
		ParNode* firstFar = (ParNode*) farS->myMaster();
		makeConnection(pn, farN, p, firstFar);
	} else if (!farN) {
		// if no output is connected to a star 
		// assigned, special treatment
		makeBoundary(pn, p);
	}
}

// generate the unique name for "Spread", "Collect", "Receive", and "Send".
const char* newName(int flag) {
	static int id = 0;

	char buf[20];
	if (flag == 1) {
		sprintf(buf, "spread%d", id++);
	} else if (flag == 0) {
		sprintf(buf, "collect%d", id++);
	} else if (flag == 2) {
		sprintf(buf, "receive%d", id++);
	} else {
		sprintf(buf, "send%d", id++);
	}

	if (id > 10000) id = 0;

	return hashstring(buf);
}
	
// find a porthole with a given name and set the numberTokens parameter
PortHole* findPortHole(DataFlowStar* s, const char* n, int num) {
	DFPortHole* p = (DFPortHole*) s->portWithName(n);
	p->setSDFParams(num,num-1);
	return p;
}

// This is a trap handler for catching "impossible" conditions.
// It will bomb the program if the argument pointer is null.
// It is added because the dataflow analysis of gcc says there
// is no guarantee that sG and dG in the code below get set.

static void checkNonNull(void *ptr,const char* msg) {
	if (ptr == 0) {
		Error::abortRun("FATAL: ", msg);
		exit (1);
	}
}

///////////////////
// makeConnection 
///////////////////

void UniProcessor :: makeConnection(ParNode* dN, ParNode* sN, 
				    PortHole* ref, ParNode* firstS) {
	PortHole* farP = ref->far();

	// Now make connections.
	// STEP1: get the cloned stars and identify portholes

	DataFlowStar* src = 0;
	PortHole* srcP = 0;
	DataFlowStar* dest = dN->getCopyStar();
	PortHole* destP = dest->portWithName(ref->name());

	if (sN) {
		src = sN->getCopyStar();
		srcP = src->portWithName(farP->name());
	}
	
	// temporaries.
	ParNode* partner;
	EGGate *sG = 0, *dG = 0;

	// STEP2:
	// check whether Spread node and Collect node is necessary or not.

	int spreadReq = 0;
	int collectReq = 0;

	// number of connections, and comm.
	int numCon = 0, numRecv = 0, numSend = 0;	
	int numDelay = 0;

	// step through the connections starting from the first invocation
	// of the source star, which guarantees the right ordering of the
	// data produced.
	ParNode* n = firstS;
	do {
		int sID = n->getProcId();
		EGGateLinkIter descs(n->descendants);
		EGGate* g;
		while ((g = descs++) != 0) {
			if (g->samples() == 0) continue;
			if (strcmp(g->name(), farP->name())) continue;
			
			// get the partner node.
			partner = (ParNode*) g->farEndNode();
			int dID = partner->getProcId();

			if ((sID == myId()) && (dID == myId())) {
				numCon++;
				numDelay += g->delay();
				if (spreadReq < 0) spreadReq = TRUE;
				if (collectReq < 0) collectReq = TRUE;
			} else if (sID == myId()) {
				sG = g;
				numSend++;
				if ((numCon > 0) && (!collectReq))
					collectReq = -1;
			} else if (dID == myId()) {
				numRecv++;
				dG = g->farGate();
				numDelay += g->delay();
				if ((numCon > 0) && (!spreadReq)) 
					spreadReq = -1;
			}
		}
		n = (ParNode*) n->getNextInvoc();
	} while (n);

	if (numRecv > 0) {
		if (numRecv + numCon > 1) collectReq = TRUE;
		else {
			checkNonNull(dG,"CodeGen.cc: dG");
			partner = (ParNode*) dG->farEndNode();
			makeReceive(partner->getProcId(),destP,numDelay,dG);
		}
	}
	if (numSend > 0) {
		if (numSend + numCon > 1) spreadReq = TRUE;
		else {
			checkNonNull(sG,"CodeGen.cc: sG");
			partner = (ParNode*) sG->farEndNode();
			makeSend(partner->getProcId(),srcP,sG);
		}
	}

	// STEP3: create Spread and Collect star if necessary and make
	//	  connections between these stars and source/dest stars.

	DataFlowStar* newSpread = 0;
	DataFlowStar* newCollect = 0;
	if (spreadReq < 0) spreadReq = 0;
	if (collectReq < 0) collectReq = 0;
	if (spreadReq == TRUE) 
		newSpread =  makeSpread(srcP,sN,farP->numXfer()); 
	if (collectReq == TRUE) 
		newCollect = makeCollect(destP,dN,ref->numXfer());

	// STEP4: depending on the situation, we make connections
	// Note that delay is added on the arc of the destinations 
	// if IPC occurs.
	
	PortHole* sP, *rP;
	int numSamples = 0;
	int numDel = 0;

	// (1) neither Spread nor Collect
	if ((!spreadReq) && (!collectReq)) {
		if (numCon > 0) srcP->connect(*destP,numDelay);

	// (2) Spread only
	} else if (spreadReq && (!collectReq)) {
		n = sN;
		do {
			EGGateLinkIter descs(n->descendants);
			EGGate* g;
			while ((g = descs++) != 0) {
				if (g->samples() == 0) continue;
				if (strcmp(g->name(),farP->name()))
					continue;
				partner = (ParNode*) g->farEndNode();
				int parId = partner->getProcId();
				if (parId == myId()) {
					numSamples += g->samples();
					numDel += g->delay();
				} else {
					// make connection to the partner if
					// connection exists.
					if (numSamples > 0) {
					   sP = findPortHole(newSpread,
						"output", numSamples);
					   sP->connect(*destP,numDel);
					   numSamples = 0;
					   numDel = 0;
					}
					// create Send star
					sP = findPortHole(newSpread, "output",
					     g->samples());
					makeSend(parId,sP,g);
				}
			}
			n = n->getNextNode();
		} while (n);
		if (numSamples > 0) { 	// last connection
			sP = findPortHole(newSpread, "output", numSamples);
			sP->connect(*destP, numDel);
		}

	// (3) Collect only
	} else if ((!spreadReq) && collectReq) {
		n = firstS;
		do {
			int sID = n->getProcId();
			EGGateLinkIter descs(n->descendants);
			EGGate* g;
			while ((g = descs++) != 0) {
				if (g->samples() == 0) continue;
				if (strcmp(g->name(),farP->name())) continue;

				partner = (ParNode*) g->farEndNode();
				if (partner->getProcId() != myId()) continue;

				numDel += g->delay();
				if (sID == myId()) {
					numSamples += g->samples();
				} else {
					// make connection to the partner if
					// connection exists.
					if (numSamples > 0) {
					   rP = findPortHole(newCollect, 
						"input", numSamples);
					   srcP->connect(*rP,0);
					   numSamples = 0;
					}
					// create Receive star
					rP = findPortHole(newCollect, 
						"input", g->samples());
					makeReceive(sID,rP,0,g->farGate());
				}
			}
			n = (ParNode*) n->getNextInvoc();
		} while (n);
		if (numSamples > 0) { 	// last connection
			rP = findPortHole(newCollect, "input", numSamples);
			srcP->connect(*rP, 0);
		}
		// set the delay parameter
		destP->setDelay(numDel);

	// (4) Both
	} else {	// both flags are on.
		n = firstS;
		do {
			int sID = n->getProcId();
			EGGateLinkIter descs(n->descendants);
			EGGate* g;
			while ((g = descs++) != 0) {
				if (g->samples() == 0) continue;
				if (strcmp(g->name(),farP->name())) continue;

				partner = (ParNode*) g->farEndNode();
				int dID = partner->getProcId();

				if ((sID != myId())&&(dID != myId())) continue;

				if (sID == dID) {
					numSamples += g->samples();
					numDel += g->delay();
				} else {
					if (numSamples > 0) {
						// connect Spread & Collect
						sP = findPortHole(newSpread,
							"output", numSamples);
					   	rP = findPortHole(newCollect,
							"input", numSamples);
					   	sP->connect(*rP,0);
						numSamples = 0;
					}
					if (sID == myId()) {
						// create Send star
						sP=findPortHole(newSpread, 
							"output",g->samples());
						makeSend(dID,sP,g);
					} else {
						numDel += g->delay();
						// create Receive star
						rP = findPortHole(newCollect, 
							"input",g->samples());
						dG = g->farGate();
						makeReceive(sID,rP,0,dG);
					}
				}
			}
			n = (ParNode*) n->getNextInvoc();
		} while (n);

		if (numSamples > 0) { 	// last connection
			sP = findPortHole(newSpread, "output", numSamples);
			rP = findPortHole(newCollect, "input", numSamples);
			sP->connect(*rP,0);
		}

		// set the delay parameter
		destP->setDelay(numDel);
	}
}
							

///////////////////////////////
// makeReceive and makeSend
///////////////////////////////

// create a Receive star or a Send star and connect it to the specified port.
 
void 
UniProcessor :: makeReceive(int pindex, PortHole* rP, int delay, 
			    EGGate* g, PortHole* orgP) {
	int numSample;
	if (orgP != 0) {
		numSample = orgP->far()->numXfer();
	}
	else {
		numSample = rP->numXfer();
	}	
	// create target specific Receive star
	DataFlowStar* newR = mtarget->createReceive(pindex,myId(),numSample);
	newR->setName("CGReceive_");
	newR->setTarget(targetPtr);
	subGal->addBlock(*newR,newName(2));

	// make connection
	PortHole* sP = newR->portWithName("output");
	sP->connect(*rP, delay);
	((DFPortHole*)sP)->setSDFParams(numSample,numSample-1);
	newR->repetitions = ((DFPortHole*)rP)->parentReps();
	   
	// set the cloned star pointer of the receive nodes
	int comp = myId() - pindex;
	ParNode* pn = parent->matchCommNodes(newR, g, orgP);
	// pair Send and Receive star
	if ((comp > 0) && (pn != 0)) 
	    mtarget->pairSendReceive(pn->getPartner()->getCopyStar(), newR);
}

// Note that the delay is attached in the receiver part if any.
void UniProcessor :: makeSend(int pindex, PortHole* sP, 
			EGGate* g, PortHole* orgP) {
	int numSample;

	if (orgP != 0) {
		numSample = orgP->numXfer();
	}
	else {
		numSample = sP->numXfer();
	}		
	// create target specific Send star
	DataFlowStar* newS = mtarget->createSend(myId(), pindex, numSample);
	newS->setName("CGSend_");
	newS->setTarget(targetPtr);
	subGal->addBlock(*newS,newName(3));

	// make connection
	PortHole* rP = newS->portWithName("input");
	sP->connect(*rP, 0);
	((DFPortHole*)rP)->setSDFParams(numSample,numSample-1);
	newS->repetitions = ((DFPortHole*)sP)->parentReps();

	// set the cloned star pointer of the Send node
	int comp = myId() - pindex;
	ParNode* pn = parent->matchCommNodes(newS, g, orgP);
	// pair Send and Receive star
	if ((comp > 0)  && (pn != 0))
	    mtarget->pairSendReceive(newS,pn->getPartner()->getCopyStar());
}
	
///////////////////////////////
// makeSpread and makeCollect
///////////////////////////////

// create a Spread star and connect it to the source porthole.

DataFlowStar* UniProcessor :: makeSpread(PortHole* srcP, ParNode* sN, int n) {
	DataFlowStar* newSpread = mtarget->createSpread();
	newSpread->setTarget(targetPtr);
	subGal->addBlock(*newSpread,newName(1));
	int numTok;
	if (sN)
		numTok = n * sN->numAssigned();
	else
		numTok = n;
	PortHole* p = findPortHole(newSpread, "input", numTok);
	srcP->connect(*p,0);
	return newSpread;
}

// create a Collect star and connect it to the destination porthole.

DataFlowStar* UniProcessor :: makeCollect(PortHole* destP, ParNode* dN,int n) {
	DataFlowStar* newCollect = mtarget->createCollect();
	newCollect->setTarget(targetPtr);
	subGal->addBlock(*newCollect,newName(0));
	int numTok;
	if (dN)
		numTok = n * dN->numAssigned();
	else
		numTok = n;
	PortHole* p = findPortHole(newCollect, "output", numTok);
	p->connect(*destP,0);
	return newCollect;
}

/////////////////
// makeBoundary 
/////////////////

// This porthole is connected to only Send stars.
void UniProcessor :: makeBoundary(ParNode* sN, PortHole* ref) {
	// check whether Spread is necessary or not.
	EGGate* sG = 0;
	ParNode* n = sN;
	ParNode* partner = 0;
	int count = 0;
	do {
		EGGateLinkIter descs(n->descendants);
		EGGate* g;
		while ((g = descs++) != 0) {
			if (g->samples() == 0) continue;
			if (strcmp(g->name(), ref->name())) continue;
			partner = (ParNode*) g->farEndNode();
			sG = g;
			count++;
			if (count > 1) goto M;
		}
		n = n->getNextNode();
	} while(n);

M:
	DataFlowStar* src = sN->getCopyStar();
	PortHole* srcP = src->portWithName(ref->name());

	DataFlowStar* newSpread = 0;
	if (count > 1) {
		newSpread = makeSpread(srcP, sN, ref->numXfer());
	}

	// make connections.
	if (!newSpread) { // only one send star
		// create Send star
		makeSend(partner->getProcId(),srcP, sG);
		return;
	}
	
	// more than one send star
			
	n = sN;
	do {
		EGGateLinkIter descs(n->descendants);
		EGGate* g;
		while ((g = descs++) != 0) {
			if (g->samples() == 0) continue;
			if (strcmp(g->name(), ref->name())) continue;
			partner = (ParNode*) g->farEndNode();

			PortHole* sP = findPortHole(newSpread, "output", 
				g->samples());
			makeSend(partner->getProcId(),sP,g);
		}
		n = n->getNextNode();
	} while(n);
}
