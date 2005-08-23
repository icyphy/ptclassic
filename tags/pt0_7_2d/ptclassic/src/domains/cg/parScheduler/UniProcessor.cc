static const char file_id[] = "UniProcessor.cc";

/*****************************************************************
Version identification:
@(#)UniProcessor.cc	1.33	3/19/96

Copyright (c) 1990-1996 The Regents of the University of California.
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
Date of last revision: 

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "UniProcessor.h"
#include "SDFScheduler.h"
#include <iostream.h>
#include "ganttChart.h"
#include "StringList.h"
#include "InfString.h"

StringList UniProcessor :: display(int makespan)
{
    StringList schedule;
    sumIdle = makespan - availTime;

    // iterator for the scheduled node
    ProcessorIter schedIter(*this);
    NodeSchedule* obj;

    while((obj = schedIter++) != 0) {
	ParNode* node = (ParNode*) obj->getNode();
	if (obj->isIdleTime()) {	// idle node
	    schedule << "    { idle ";
	    sumIdle += obj->getDuration();
	} else {
	    if (node->myStar())
		schedule << "    { fire "
		         <<  node->myStar()->fullName()
			 << " ";
	    else if (node->getType() == -1)
		schedule << "    { fire send ";
	    else
		schedule << "    { fire receive ";
	}
	schedule << "{ exec_time " << obj->getDuration() << "} }\n";
    }

    StringList out;
    out << "{\n";
    if (target())
	out << "    { target " << target()->name() << " }\n";
    out << "    { totalIdleTime " << sumIdle << " }\n";
    out << schedule;
    out << "  }\n";

    return out;
}

// write Gantt chart.
int UniProcessor :: writeGantt(ostream& out, const char* universe, int numProcs, int span) {

	int total = 0;
	int start = 0;
	int fin = 0;
	int proc_num = 0;

	ProcessorIter iter(*this);
	NodeSchedule* obj;

	proc_num = ++(this->index);

	while ((obj = iter++) != 0) {
		ParNode* node = (ParNode*) obj->getNode();
		int nType = node->getType();
		if (nType <= 0) { // Not an idle star
			total += obj->getDuration();
			fin = start + obj->getDuration();

			InfString tmpbuf;
			const char* starName;
			if (!nType) {
				starName = node->myStar()->name();
			} else if (nType == -1) {
				starName = "snd";
			} else {
				starName = "rcv";
			}
			tmpbuf = "ptkGantt_DrawProc ";
			tmpbuf << universe << " " << numProcs << " ";
			tmpbuf << span << " " << proc_num << " " << starName;
			tmpbuf << " " << start << " " << fin;
			GanttChart::writeGanttProc(out, (char *)tmpbuf);
			start = fin;
		} else { // Idle node
			start += obj->getDuration();
		}
	}
	return total;
}

	///////////////////////
	// Schedule routine
	//////////////////////

static ParNode idleNode(1);

// schedule a node at the end
// Return the completion time.
int UniProcessor :: schedAtEnd(ParNode* pd, int when, int leng) {

	// schedule idle time if any.
	int idle = when - getAvailTime();
	if (idle > 0)
		appendNode(&idleNode, idle);

	// assign node
	appendNode(pd, leng);
	pd->setScheduledTime(when);
	pd->setFinishTime(when+leng);

	setAvailTime(when+leng);
	return 0;
}
	
// Check whether we can schedule a node in an idle slot if any.
int UniProcessor :: filledInIdleSlot(ParNode* node, int start, int limit) {

	int load;
	if (!node->myMaster()) load = node->getExTime();
	else load = mtarget->execTime(node->myMaster(), targetPtr);
	if (load < 0) return -1;

	int curTime = getAvailTime();
	NodeSchedule* obj = (NodeSchedule*) tail();
	ParNode* temp;
	int prevTime = -1;
	if (limit >= curTime) prevTime = curTime;

	while (obj) {
		curTime -= obj->getDuration();
		temp = obj->getNode();
		if (obj->isIdleTime() == 0) { // regular or visible comm node
			if (curTime < start + load) {
				return prevTime;
			}  else if ((curTime == start) && (load == 0)) {
				return start;
			}
		} else { // idle node or comm node which is hidden.
			if (obj->getDuration() >= load) {
				if (curTime <= start) 
					return start;
				else if ((limit == 0) || (curTime <= limit))
					prevTime = curTime;
			} 
		}
		obj = (NodeSchedule*) obj->previousLink();
	}
	return prevTime;
}

// schedule in the middle (during an idle slot)
// return the completion time if succeeded, else "-1"
int UniProcessor :: schedInMiddle(ParNode* pd, int when, int leng) {

	int curTime = getAvailTime();
	NodeSchedule* obj = (NodeSchedule*) tail();
	ParNode* temp = obj->getNode();

	while (obj) {
		curTime -= obj->getDuration();
		temp = obj->getNode();

		// special case
		if ((curTime == when) && (leng == 0)) {
			NodeSchedule* n = getFree();
			n->setMembers(pd,leng);
			insertAhead(n,obj);	
			pd->setScheduledTime(when);
			pd->setFinishTime(when);
			return when;
	
		// check the current node should be an idle node.
		} else if (obj->isIdleTime() == 0) {  
			if (curTime < when + leng) {
				return -1;
			}
		} else if ((curTime <= when) && (obj->getDuration() >= leng)) {
			int endSlot = curTime + obj->getDuration();	

			// schedule the node.
			// 1. get the next link
			NodeSchedule* n = (NodeSchedule*) obj->nextLink();
			// 2. remove idle link
			unlink(obj);
			putFree(obj);
			// 3. add idle node if necessary
			int x = endSlot - when - leng;
			if (x > 0) {
				obj = getFree();
				obj->setMembers(&idleNode,x);
				insertAhead(obj,n);
			} else {
				obj = n;
			}

			// schedule node
			n = getFree();
			n->setMembers(pd,leng);
			insertAhead(n,obj);
			pd->setScheduledTime(when);
			pd->setFinishTime(when + leng);

			// schedule idle time if any.
			x = when - curTime;
			if (x > 0) {
			obj = getFree();
			obj->setMembers(&idleNode,x);
			insertAhead(obj,n);
			}

			return when+leng;
		}
		obj = (NodeSchedule*) obj->previousLink();
	}
	return -1;
}

// Assign nodes into the processor
void UniProcessor :: appendNode(ParNode* n, int val) {
	NodeSchedule* ns = getFree();
	ns->setMembers(n, val);
	appendLink(ns);
	curSchedule = ns;
}

			//////////////////////
			///  scheduleComm  ///
			//////////////////////

// schedule a communication node
void UniProcessor :: scheduleCommNode(ParNode* n, int start) {
	// We check when this node can be scheduled in this
	// Processor and in the Communication link simultaneously.
	// To do that, we detect the time slots in this processor to
	// schedule the node first, and check whether these time slots
	// are available in the communication link. 
	if (start < availTime) {
		int load = n->getExTime();
		int curTime = 0;
		ProcessorIter iter(*this);
		NodeSchedule* obj;

		while ((obj = iter++) != 0) {
			if (obj->isIdleTime() == FALSE) {
				curTime += obj->getDuration();
				continue;
			}
			int from = (curTime < start)? start: curTime;
			curTime += obj->getDuration();
			if (curTime - from >= load) {
				int limit = curTime - load;
				int t = mtarget->scheduleComm(n,from,limit);
				if (t > 0) {
					addNode(n,from);
					return;
				}
			}
		}
		start = availTime;
	}	
	int t = mtarget->scheduleComm(n, start);
	if (t > start) start = t;
	addNode(n, start);
}

			/////////////////
			///  addNode  ///
			/////////////////
// Adds the node to the schedule sometime after start
// It looks through the gaps and tries to fill them in if it can.

void UniProcessor::addNode(ParNode *node, int start) {

	// If earliestStart is >= availTime, then we append the node
	int d;
	if (!node->myMaster()) d = node->getExTime();
	else d = mtarget->execTime(node->myMaster(), targetPtr);

	if (start < availTime) {
		// start < availTime so check gaps
		int newStart = filledInIdleSlot(node,start);
		if (newStart >= 0) {
			if (schedInMiddle(node,newStart,d) < 0)
				Error::abortRun("no enough idle slot!");
		} else {
			schedAtEnd(node,availTime,d);
		}
	} else {
		schedAtEnd(node,start,d);
	}
}

// return the start time of the schedule
int UniProcessor :: getStartTime() {

	int idleTime = 0;

	// iterator for the scheduled node
	ProcessorIter schedIter(*this);
	NodeSchedule* obj;

	while((obj = schedIter++) != 0) {
		if (obj->isIdleTime()) {	// idle node
			idleTime += obj->getDuration();
		} else {
			return idleTime;
		}
	}
	return 0;
}

		/////////////////////////////////////
		//    Prepare code generation
		/////////////////////////////////////

void UniProcessor :: prepareCodeGen() {
	// galaxy initialize
	subGal->initialize();

	// convert a processor schedule to a SDF schedule
	// for child target.
	targetPtr->setGalaxy(*subGal);
	convertSchedule(0);
	if (SimControl::haltRequested()) return;

	// simulate the schedule
	simRunSchedule();
	if (SimControl::haltRequested()) return;
}
	
// convert the UniProcessor schedule to SDFSchedule. The optional
// SDFScheduler to be converted into is provided.
void UniProcessor :: convertSchedule(SDFScheduler* optS) {

	SDFSchedule sched;
	ProcessorIter schedIter(*this);
	ParNode* n;
	while ((n = schedIter.nextNode()) != 0) {
		DataFlowStar* s;
		if (n->getType() > 0) continue;
		if (n->getType() < 0) {
			s = n->getCopyStar();

		// If the node is associated with a parallel star, 
		// first check if it is the syncProc of the star.
		// If not, get copy star.

		} else if (n->amIBig() && (n->getProcId() != myId())) {
			s = n->copyStar(target(), myId(), 0);
			if (!s) return;
		} else {
			s = n->getCopyStar();
		}
		s->setTarget(targetPtr);
		sched.append(*s);
	}
	if (optS == 0) {
		targetPtr->scheduler()->setGalaxy(*subGal);
		targetPtr->copySchedule(sched);
	} else {
		optS->copySchedule(sched);
	}
}

// trace the schedule to obtain the right buffer size
void UniProcessor :: simRunSchedule() {
	ProcessorIter iter(*this);
	ParNode* n;
	
	while ((n = iter.nextNode()) != 0) {
		if ((n->getType() > 0) || (n->getProcId() != myId())) continue;
		DataFlowStar* copyS = n->getCopyStar();

		DFStarPortIter piter(*copyS);
		CGPortHole* p;
		while ((p = (CGPortHole*) piter++) != 0) {
			if (p->far() == NULL) continue;
			// in case of embedding
			if (p->embedded() || p->embedding()) continue;
			CGPortHole* pFar = (CGPortHole*) p->far();
			if (pFar->embedded() || pFar->embedding())
				p->incCount(p->numXfer());
			else if (p->isItInput())
				p->decCount(p->numXfer());
			else
				p->incCount(p->numXfer());
		}
	}
}

		/////////////////////////////////////
		//    Generate code
		/////////////////////////////////////

StringList& UniProcessor :: generateCode() {
	targetPtr->generateCode();
	
	return (*targetPtr->getStream("code"));
}

// Insert the subGalaxy code to the argument target class.

int UniProcessor :: genCodeTo(Target* t) {
	targetPtr = (CGTarget*) t;

	// galaxy initialize
	subGal->initialize();

	// convert a processor schedule to a SDF schedule
	// for child target.
	SDFScheduler tempSched;
	tempSched.setGalaxy(*subGal);
	convertSchedule(&tempSched);
	if (SimControl::haltRequested()) return FALSE;

	// simulate the schedule
	simRunSchedule();
	if (SimControl::haltRequested()) return FALSE;

	if (!targetPtr->insertGalaxyCode(subGal, &tempSched)) return FALSE;
	return TRUE;
}

		/////////////////////////////////////
		//    destructor
		/////////////////////////////////////

// destructor
UniProcessor :: ~UniProcessor() {
	LOG_DEL; delete subGal;
	initialize();
	clearFree();
}

//////////////////////
// Memory management
//////////////////////

NodeSchedule* UniProcessor :: getFree() {

	NodeSchedule *temp; 
	if (freeNodeSched) {
		temp = freeNodeSched;
		freeNodeSched = temp->nextFree;
		temp->resetMembers();
		numFree--;
	} else {
		LOG_NEW; temp = new NodeSchedule();
	}
	return temp;
}

void UniProcessor :: putFree(NodeSchedule* obj) {

	obj->nextFree = freeNodeSched;
	freeNodeSched = obj;
	numFree++;
}

void UniProcessor :: clearFree() {

	if (!freeNodeSched) return;
	NodeSchedule* temp;
	while(freeNodeSched->nextFree) {
		temp = freeNodeSched;
		freeNodeSched = freeNodeSched->nextFree;
		LOG_DEL; delete temp;
	}
	LOG_DEL; delete freeNodeSched;
	freeNodeSched = 0;
	numFree = 0;
}

void UniProcessor :: initialize() {

	ProcessorIter pIter(*this);
	NodeSchedule* obj;

	while ((obj = pIter++) != 0) {
		unlink(obj);
		putFree(obj);
	}
	curSchedule = 0;
	availTime = 0;
	reset();
}

//////////////////////
// copying 
//////////////////////

void UniProcessor :: copy(UniProcessor* org) {

	initialize();
	ProcessorIter pIter(*org);
	NodeSchedule* obj;

	while((obj = pIter++) != 0) {
		appendNode(obj->getNode(), obj->getDuration());
	}
	setAvailTime(org->getAvailTime());
}

NodeSchedule* UniProcessor :: getNodeSchedule(ParNode* n) {
	ProcessorIter proc(*this);
	NodeSchedule* ns;
	while ((ns = proc++) != 0)
		if (ns->getNode() == n) return ns;
	return 0;
}

                        /////////////////////
                        ///  computeLoad  ///
                        /////////////////////
// Returns the sum of the computation + communication load on this processor.
int UniProcessor::computeLoad() {

	int totalLoad = 0;

	ProcessorIter schedIter(*this);
	NodeSchedule* obj;
	while ((obj = schedIter++) != 0) {
		ParNode* pn = obj->getNode();
		if (pn->getType() <= 0) totalLoad += pn->getExTime();
	}
	load = totalLoad;       // Set private data member
	return totalLoad;
}

