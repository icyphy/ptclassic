static const char file_id[] = "CodeGen.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision: 

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

static void copyActualStates(const Block& src, Block& dest) {
	CBlockStateIter nexts(src);
	BlockStateIter nextd(dest);
	const State* srcStatePtr;
	State *destStatePtr;
	while ((srcStatePtr = nexts++) != 0 && (destStatePtr = nextd++) != 0)
		destStatePtr->setValue(
			hashstring(srcStatePtr->currentValue()));
}

// clone a star
DataFlowStar* cloneStar(DataFlowStar* org) {
	DataFlowStar* newS = (DataFlowStar*) org->clone();
	copyActualStates(*org, *newS);
	if (org->numberMPHs() <= 0) return newS;
	
	// clone the multi portholes.
	BlockMPHIter piter(*org);
	MultiPortHole* p;
	while ((p = piter++) != 0) {
		MultiPortHole* cP = newS->multiPortWithName(p->name());
		for (int i = p->numberPorts(); i > 0; i--)
			cP->newPort();
	}
	return newS;
}

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

	// maintain the list of the SDF stars which we have considered
	SequentialList touchedStars;
	touchedStars.initialize();

	// maintain the earliest invocation of a star assigned.
	SequentialList assignedFirstInvocs;
	assignedFirstInvocs.initialize();
	
	// create SDF stars.
	ProcessorIter pIter(*this);
	ParNode* n;
	while ((n = pIter.nextNode()) != 0) {
		if (n->getType()) continue;

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

		DataFlowStar* copyS = cloneStar(org);
		copyS->setTarget(targetPtr);

		ParNode* prevN = 0;
		smallest->setCopyStar(copyS, prevN);
		prevN = smallest;

		copyS->setMaster(smallest);
		assignedFirstInvocs.put(smallest);

		// add to the galaxy
		subGal->addBlock(*copyS, org->name());

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
		while ((p = piter++) != 0) {
			// wormhole boundary
			if (p->atBoundary()) {
				PortHole* cp = clonedPort(org, p);
				PortHole* evep = p->far();
				p->geo()->disconnect(*evep);
				if (p->isItInput()) {
					evep->connect(*cp,p->numTokens());
				} else {
					cp->connect(*evep,p->numTokens());
				}
				continue;
			}

			DataFlowStar* farS =(DataFlowStar*) p->far()->parent();
			ParNode* farN = (ParNode*) farS->myMaster();
			ParNode* myN = (ParNode*) org->myMaster();
			
			// depending on OSOPreq()
			if (OSOPreq() || (myN->isOSOP() && farN->isOSOP())) {
				makeOSOPConnect(p, org, farS, touchedStars);
			} else {
				makeGenConnect(p, pn, org, farS, touchedStars);
			}
		}
	}
}

			////////////////////////////
			// makeOSOPConnect
			////////////////////////////

void UniProcessor :: makeOSOPConnect(PortHole* p, DataFlowStar* org, 
	DataFlowStar* farS, SequentialList& touchedStars) {

	if (touchedStars.member(farS)) {
	 	// make connection if it is output
		if (p->isItOutput()) {
			PortHole* destP = clonedPort(farS, p->far());
			clonedPort(org, p)->connect(*destP, p->numTokens());
		}
	} else {
		ParNode* n = (ParNode*) farS->myMaster();
		PortHole* cp = clonedPort(org, p);
		if (p->isItInput()) {
			makeReceive(n->getProcId(),cp,p->numTokens(),0,p);
		} else {
			makeSend(n->getProcId(), cp, 0, p);
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
// FIXME: need DFPortHole methods to fix SDF parameters.  This should
// use DFPortHole instead.
// also, is 0 right for maxDelay?
PortHole* findPortHole(DataFlowStar* s, const char* n, int num) {
	SDFPortHole* p = (SDFPortHole*) s->portWithName(n);
	p->setSDFParams(num,0);
	return p;
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
	EGGate *sG, *dG;

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
			partner = (ParNode*) dG->farEndNode();
			makeReceive(partner->getProcId(),destP,numDelay,dG);
		}
	}
	if (numSend > 0) {
		if (numSend + numCon > 1) spreadReq = TRUE;
		else {
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
	if (spreadReq == TRUE) newSpread =  makeSpread(srcP,sN); 
	if (collectReq == TRUE) newCollect = makeCollect(destP, dN);

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
	int numSample = rP->numXfer();
	
	// create target specific Receive star
	DataFlowStar* newR = mtarget->createReceive(pindex,myId(),numSample);
	newR->setTarget(targetPtr);
	subGal->addBlock(*newR,newName(2));

	// make connection
	PortHole* sP = newR->portWithName("output");
	sP->connect(*rP, delay);

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
	int numSample = sP->numXfer();
	
	// create target specific Send star
	DataFlowStar* newS = mtarget->createSend(myId(), pindex, numSample);
	newS->setTarget(targetPtr);
	subGal->addBlock(*newS,newName(3));

	// make connection
	PortHole* rP = newS->portWithName("input");
	sP->connect(*rP, 0);

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

DataFlowStar* UniProcessor :: makeSpread(PortHole* srcP, ParNode* sN) {
	DataFlowStar* newSpread = mtarget->createSpread();
	newSpread->setTarget(targetPtr);
	subGal->addBlock(*newSpread,newName(1));
	int numTok;
	if (sN)
		numTok = srcP->numXfer() * sN->numAssigned();
	else
		numTok = srcP->numXfer();
	PortHole* p = findPortHole(newSpread, "input", numTok);
	srcP->connect(*p,0);
	return newSpread;
}

// create a Collect star and connect it to the destination porthole.

DataFlowStar* UniProcessor :: makeCollect(PortHole* destP, ParNode* dN) {
	DataFlowStar* newCollect = mtarget->createCollect();
	newCollect->setTarget(targetPtr);
	subGal->addBlock(*newCollect,newName(0));
	int numTok;
	if (dN)
		numTok = destP->numXfer() * dN->numAssigned();
	else
		numTok = destP->numXfer();
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
	EGGate* sG;
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
		newSpread = makeSpread(srcP, sN);
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

