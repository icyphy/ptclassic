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
#include "CGCollect.h"
#include "CGSpread.h"
#include "KnownBlock.h"
#include "SDFConnect.h"

// clone a star
SDFStar* cloneStar(SDFStar* org) {
	SDFStar* newS = (SDFStar*) org->clone();
	/* NOT DEFINED YET newS->copyStates(*org); */
	if (org->numberMPHs() <= 0) return newS;
	
	// clone the multi portholes.
	BlockMPHIter piter(*org);
	MultiPortHole* p;
	while ((p = piter++) != 0) {
		MultiPortHole* cP = newS->multiPortWithName(p->readName());
		for (int i = p->numberPorts(); i > 0; i--)
			cP->newPort();
	}
	return newS;
}

			//////////////////////
			// sub-Galaxy creation
			//////////////////////

void UniProcessor :: createSubGal() {

	// create data structure
	LOG_DEL; delete subGal;
	LOG_NEW; subGal = new DynamicGalaxy;
	subGal->setNameParent(targetPtr->readName(), 0);

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
		SDFStar* org = n->myMaster();

		// If already dealt with, continue.
		if (touchedStars.member(org)) continue;
		// We believe temporal locality
		touchedStars.tup(org);

		ParNode* smallest = (ParNode*) org->myMaster();
		while (smallest && (smallest->getProcId() != myId()))
			smallest = (ParNode*) smallest->getNextInvoc();

		SDFStar* copyS = cloneStar(org);

		ParNode* prevN = 0;
		smallest->setCopyStar(copyS, prevN);
		prevN = smallest;

		copyS->setMaster(smallest);
		assignedFirstInvocs.put(smallest);

		// add to the galaxy
		subGal->addBlock(*copyS, org->readName());

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
		SDFStar* org = pn->myMaster();
		BlockPortIter piter(*org);
		PortHole* p;
		while ((p = piter++) != 0) {
			SDFStar* farS = (SDFStar*) p->far()->parent();
			
			// depending on OSOPreq()
			if (OSOPreq()) {
				makeOSOPConnect(p, org, farS, touchedStars);
			} else {
				makeGenConnect(p, pn, org, farS, touchedStars);
			}
		}
	}
}

PortHole* clonedPort(SDFStar* s, PortHole* p) {
	ParNode* n = (ParNode*) s->myMaster();
	SDFStar* copyS = n->getCopyStar();
	return copyS->portWithName(p->readName());
}

			////////////////////////////
			// makeOSOPConnect
			////////////////////////////

void UniProcessor :: makeOSOPConnect(PortHole* p, SDFStar* org, SDFStar* farS,
				     SequentialList& touchedStars) {

	if (touchedStars.member(farS)) {
	 	// make connection if it is input
		if (p->isItInput()) {
			PortHole* destP = clonedPort(farS, p->far());
			clonedPort(org, p)->connect(*destP, p->numTokens());
		}
	} else {
		ParNode* sn = (ParNode*) org->myMaster();
		ParNode* n = (ParNode*) farS->myMaster();
		PortHole* cp = clonedPort(org, p);
		if (p->isItInput()) {
			makeReceive(n->getProcId(), cp, p->numTokens(), sn, 0);
		} else {
			makeSend(n->getProcId(), cp, sn, 0);
		}
	}
}

			///////////////////////////////
			// makeGenConnect
			///////////////////////////////

void UniProcessor :: makeGenConnect(PortHole* p, ParNode* pn, SDFStar* org,
			  SDFStar* farS, SequentialList& touchedStars) {

	ParNode* farN = 0;
	if (touchedStars.member(farS)) {
		farN = (ParNode*) farS->myMaster();
		while (farN->getProcId() != myId())
			farN = (ParNode*) farN->getNextInvoc();
	}
	if (p->isItInput())
		makeConnection(pn, farN, p);
	else if (!farN)
		// if no output is connected to a star 
		// assigned, special treatment
		makeBoundary(pn, p);
}

// generate the unique name for "Spread", "Collect", "Receive", and "Send".
const char* newName(int flag) {
	static int id = 0;

	char buf[20];
	if (flag == 1) {
		sprintf(buf, "scatter%d", id++);
	} else if (flag == 0) {
		sprintf(buf, "gather%d", id++);
	} else if (flag == 2) {
		sprintf(buf, "receive%d", id++);
	} else {
		sprintf(buf, "send%d", id++);
	}

	if (id > 10000) id = 0;

	return hashstring(buf);
}
	
// find a porthole with a given name and set the numberTokens parameter
PortHole* findPortHole(SDFStar* s, const char* n, int num) {
	SDFPortHole* p = (SDFPortHole*) s->portWithName(n);
	p->setSDFParams(num,0);
	return p;
}

///////////////////
// makeConnection 
///////////////////

void UniProcessor :: makeConnection(ParNode* dN, ParNode* sN, PortHole* ref) {
	ParNode* n = dN;
	ParNode* partner;
	PortHole* farP = ref->far();
	EGGate* sG;

	// check whether Spread node and Collect node is necessary or not.
	int scatterReq = 0;
	int gatherReq = 0;

	int numCon, numRecv, numSend;
	int numDelay = 0;
	numRecv = 0;
	numCon = 0;
	do {
		EGGateLinkIter ancs(n->ancestors);
		EGGate* g;
		while ((g = ancs++) != 0) {
			if (strcmp(g->readName(), ref->readName())) continue;
			
			// get the partner node.
			partner = (ParNode*) g->farEndNode();

			if (partner->getProcId() == myId()) {
				numCon++;
				numDelay += g->delay();
				if (numRecv > 0) {
					gatherReq = TRUE;
					if (scatterReq < 0) scatterReq = TRUE;
				}
			} else {
				sG = g;
				numDelay += g->delay();
				numRecv++;
				if (numCon > 0) scatterReq = -1;
			}
		}
		n = n->getNextNode();
	} while (n);
	if (scatterReq < 0) scatterReq = FALSE;

	if ((numRecv > 0) && (numRecv + numCon > 1)) gatherReq = TRUE;

	if(sN && (!scatterReq)) {  // check additional scatter Req.
		n = sN;
		numSend = 0;
		numCon = 0;
		do {
			EGGateLinkIter descs(n->descendants);
			EGGate* g;
			while ((g = descs++) != 0) {
				if (strcmp(g->readName(),farP->readName()))
					continue;
				partner = (ParNode*) g->farEndNode();
				if (partner->getProcId() == myId()) {
					numCon++;
					if (numSend > 0) {
						scatterReq = TRUE;
						if (gatherReq < 0) 
							gatherReq = TRUE;
					}
				} else {
					numSend++;
					if ((numCon > 0) && (!gatherReq))
						gatherReq = -1;
				}
			}
			n = n->getNextNode();
		} while (n);
		if ((numSend > 0) && (numSend + numCon > 1)) scatterReq = TRUE;
	}
	if (gatherReq < 0) gatherReq = FALSE;
	
	// Now make connections.
	// STEP1: get the cloned stars and identify portholes
	SDFStar* src = 0;
	PortHole* srcP = 0;
	SDFStar* dest = dN->getCopyStar();
	PortHole* destP = dest->portWithName(ref->readName());

	if (sN) {
		src = sN->getCopyStar();
		srcP = src->portWithName(farP->readName());
	}
	
	// STEP2: create Spread and Collect star if necessary and make
	//	  connections between these stars and source/dest stars.

	SDFStar* newSpread = 0;
	SDFStar* newCollect = 0;
	if (scatterReq) newSpread =  makeSpread(srcP,sN); 
	if (gatherReq) newCollect = makeCollect(destP, dN);

	// STEP3: depending on the situation, we make connections
	// Note that delay is added on the arc of the destinations 
	// if IPC occurs.
	
	PortHole* sP, *rP;
	int numSamples = 0;
	int numDel = 0;

	// (1) neither Spread nor Collect
	if ((!scatterReq) && (!gatherReq)) {
		if (srcP) {
			srcP->connect(*destP,numDelay);
		} else {
			// create Receive star
			makeReceive(partner->getProcId(),destP,numDelay,dN,sG);
		}

	// (2) Spread only
	} else if (scatterReq && (!gatherReq)) {
		n = sN;
		do {
			EGGateLinkIter descs(n->descendants);
			EGGate* g;
			while ((g = descs++) != 0) {
				if (strcmp(g->readName(),farP->readName()))
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
					makeSend(parId,sP,n,g);
				}
			}
			n = n->getNextNode();
		} while (n);
		if (numSamples > 0) { 	// last connection
			sP = findPortHole(newSpread, "output", numSamples);
			sP->connect(*destP, numDel);
		}

	// (3) Collect only
	} else if ((!scatterReq) && gatherReq) {
		n = dN;
		do {
			EGGateLinkIter ancs(n->ancestors);
			EGGate* g;
			while ((g = ancs++) != 0) {
				if (strcmp(g->readName(),ref->readName()))
					continue;
				numDel += g->delay();
				partner = (ParNode*) g->farEndNode();
				int parId = partner->getProcId();
				if (parId == myId()) {
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
					makeReceive(parId,rP,0,n,g);
				}
			}
			n = n->getNextNode();
		} while (n);
		if (numSamples > 0) { 	// last connection
			rP = findPortHole(newCollect, "input", numSamples);
			srcP->connect(*rP, 0);
		}
		// set the delay parameter
		destP->setDelay(numDel);

	// (4) Both
	} else {	// both flags are on.
		SequentialList scatterGates;
		scatterGates.initialize();

		// first look at the source
		// Also, set the starting point of the descendants
		EGGate* firstGate = 0;
		ParNode* firstNode = 0;
		n = sN;
		do {
			EGGateLinkIter descs(n->descendants);
			EGGate* g;
			while ((g = descs++) != 0) {
				if (strcmp(g->readName(),farP->readName()))
					continue;
				partner = (ParNode*) g->farEndNode();
				int parId = partner->getProcId();
				if (parId == myId()) {
					numSamples += g->samples();
					if (!firstGate) {
						firstGate = g->farGate();
						firstNode = partner;
					}
				} else {
					// make connection to the partner if
					// connection exists.
					if (numSamples > 0) {
					   sP = findPortHole(newSpread,
						"output", numSamples);
					   scatterGates.put(sP);
					   numSamples = 0;
					}
					// create Send star
					sP=findPortHole(newSpread, "output",
						g->samples());
					makeSend(parId,sP,n,g);
				}
			}
			n = n->getNextNode();
		} while (n);
		if (numSamples > 0) { 	// last connection
			sP = findPortHole(newSpread, "output", numSamples);
			scatterGates.put(sP);
		}
		
		// next look at the destination			
		ListIter piter(scatterGates);
		sP = (PortHole*) piter++;
		int curNum = sP->numXfer();
		SDFStar* curScat = 0;

		n = firstNode;
		numSamples = 0;
		int startFlag = TRUE;
		int count = dN->numAssigned();
		int escape = FALSE;
		do {
			EGGateLinkIter ancs(n->ancestors);
			EGGate* g;
			while ((g = ancs++) != 0) {
				if (startFlag) {
					if (g != firstGate) continue;
					else startFlag = FALSE;
				}
				if (strcmp(g->readName(),ref->readName()))
					continue;
				if ((count == 0) && (g == firstGate)) {
					// jump out of the two loops, as if:
					// goto L;
					escape = TRUE;
					break;
				}

				numDel += g->delay();
				partner = (ParNode*) g->farEndNode();
				int parId = partner->getProcId();
				if (parId == myId()) {
					numSamples += g->samples();
				} else {
					// make connection to the partner if
					// connection exists.
					if (numSamples > 0) {
					   rP = findPortHole(newCollect,
						"input", numSamples);
					   if (curNum == numSamples) {
						if (curScat) {
					 	  sP = findPortHole(curScat,
							"output", numSamples);
						  curScat = 0;
						}
					   	sP->connect(*rP,0);
						sP = (PortHole*) piter++;
					   } else {
					        // create Spread star
						curScat = makeSpread(sP,0);
						curNum -= numSamples;
						sP = findPortHole(curScat,
						     "output", numSamples);
						sP->connect(*rP,0);
					   }
					   numSamples = 0;
					}
					// create Receive star
					rP = findPortHole(newCollect, "input", 
						g->samples());
					makeReceive(parId,rP,0,n,g);
				}
			}
			if (escape) break;
			n = n->getNextNode();
			if (!n) n = dN;
			count--;
		} while (count >= 0);
// L: (jump here if escaping from loop)
		if (numSamples > 0) { 	// last connection
			rP = findPortHole(newCollect, "input", numSamples);
			if (curScat) {
				sP = findPortHole(curScat,"output",numSamples);
				curScat = 0;
			}
			sP->connect(*rP, 0);
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
UniProcessor :: makeReceive(int pindex, PortHole* rP, int delay, ParNode* n,
			    EGGate* g) {
	int numSample = rP->numXfer();
	
	// create target specific Receive star
	SDFStar* newR = mtarget->createReceive(pindex,myId(),numSample);
	subGal->addBlock(*newR,newName(2));

	// make connection
	PortHole* sP = newR->portWithName("output");
	sP->connect(*rP, delay);

	int comp = myId() - pindex;
	if (g == 0) {
		matchReceiveNode(newR, rP, n, comp);
		return;
	}

	// set the cloned star pointer of the Receive node
	ParAncestorIter iter(n);
	ParNode* pn;
	while ((pn = iter++) != 0) {
		if (pn->getOrigin() == g) {
			pn->setCopyStar(newR,0);
			newR->setMaster(pn);
			// pair Send and Receive star
			if (comp > 0) mtarget->pairSendReceive(
				pn->getPartner()->getCopyStar(), newR);
			break;
		}
	}
}

// Note that the delay is attached in the receiver part if any.
void UniProcessor :: makeSend(int pindex, PortHole* sP, ParNode* n,EGGate* g) {
	int numSample = sP->numXfer();
	
	// create target specific Send star
	SDFStar* newS = mtarget->createSend(myId(), pindex, numSample);
	subGal->addBlock(*newS,newName(3));

	// make connection
	PortHole* rP = newS->portWithName("input");
	sP->connect(*rP, 0);

	int comp = myId() - pindex;
	if (g == 0) {
		matchSendNode(newS, sP, n, comp);
		return;
	}

	// set the cloned star pointer of the Send node
	ParDescendantIter iter(n);
	ParNode* pn;
	while ((pn = iter++) != 0) {
		if (pn->getOrigin() == g) {
			pn->setCopyStar(newS,0);
			newS->setMaster(pn);
			// pair Send and Receive star
			if (comp > 0) mtarget->pairSendReceive(newS, 
					pn->getPartner()->getCopyStar());
			break;
		}
	}
}
	
///////////////////////////////
// makeSpread and makeCollect
///////////////////////////////

// create a Spread star and connect it to the source porthole.

SDFStar* UniProcessor :: makeSpread(PortHole* srcP, ParNode* sN) {
	LOG_NEW; SDFStar* newSpread =  new CGSpread;
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

SDFStar* UniProcessor :: makeCollect(PortHole* destP, ParNode* dN) {
	LOG_NEW; SDFStar* newCollect = new CGCollect;
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
			if (strcmp(g->readName(), ref->readName())) continue;
			partner = (ParNode*) g->farEndNode();
			sG = g;
			count++;
			if (count > 1) goto M;
		}
		n = n->getNextNode();
	} while(n);

M:
	SDFStar* src = sN->getCopyStar();
	PortHole* srcP = src->portWithName(ref->readName());

	SDFStar* newSpread = 0;
	if (count > 1) {
		newSpread = makeSpread(srcP, sN);
	}

	// make connections.
	if (!newSpread) { // only one send star
		// create Send star
		makeSend(partner->getProcId(),srcP, sN, sG);
		return;
	}
	
	// more than one send star
			
	n = sN;
	do {
		EGGateLinkIter descs(n->descendants);
		EGGate* g;
		while ((g = descs++) != 0) {
			if (strcmp(g->readName(), ref->readName())) continue;
			partner = (ParNode*) g->farEndNode();

			PortHole* sP = findPortHole(newSpread, "output", 
				g->samples());
			makeSend(partner->getProcId(),sP,n,g);
		}
		n = n->getNextNode();
	} while(n);
}

///////////////////////////////////////
// matchReceiveNode and matchSendNode
///////////////////////////////////////

// set the cloned star pointer of the Receive node
void UniProcessor :: matchReceiveNode(SDFStar* s, PortHole* p, 
				      ParNode* n, int update) {
	// Note that n is the first invocation.
	while (n) {
		ParAncestorIter iter(n);
		ParNode* pn;
		while ((pn = iter++) != 0) {
			EGGate* orgG = pn->getOrigin();
			if (orgG)
			  if(!strcmp(orgG->readName(), p->readName())) {
				pn->setCopyStar(s,0);
				// pair Send and Receive star
				if (update > 0) {
					mtarget->pairSendReceive(pn->
					    getPartner()->getCopyStar(),s);
				}
			  }
		}
		n = (ParNode*) n->getNextInvoc();
	}
}

void UniProcessor :: matchSendNode(SDFStar* s, PortHole* p, 
				   ParNode* n, int update) {
	// Note that n is the first invocation.
	while (n) {
		ParDescendantIter iter(n);
		ParNode* pn;
		while ((pn = iter++) != 0) {
			EGGate* orgG = pn->getOrigin();
			if (orgG)
			  if(!strcmp(orgG->readName(), p->readName())) {
				pn->setCopyStar(s,0);
				// pair Send and Receive star
				if (update > 0) {
					mtarget->pairSendReceive(s,
					    pn->getPartner()->getCopyStar());
				}
			  }
		}
		n = (ParNode*) n->getNextInvoc();
	}
}

			//////////////////////
			// SimRunSchedule
			//////////////////////

// trace the schedule to obtain the right buffer size
void UniProcessor :: simRunSchedule() {
	ProcessorIter iter(*this);
	ParNode* n;
	
	while ((n = iter.nextNode()) != 0) {
		if (n->getType() != 0) continue;
		SDFStar* copyS = n->getCopyStar();

		SDFStarPortIter piter(*copyS);
		SDFPortHole* p;
		while ((p = piter++) != 0) {
			if (p->isItInput()) 
				p->decCount(p->numXfer());
			else
				p->incCount(p->numXfer());
		}
	}
}
