static const char file_id[] = "UniProcessor.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "UniProcessor.h"
#include "CGWormhole.h"

StringList UniProcessor :: display(int makespan)
{
	StringList out;
	sumIdle = makespan - availTime;
	int form = 0;

	// iterator for the scheduled node
	ProcessorIter schedIter(*this);
	NodeSchedule* obj;

	while((obj = schedIter++) != 0) {
		ParNode* node = (ParNode*) obj->getNode();
		if (obj->isIdleTime()) {	// idle node
			out += "idle (";
			sumIdle += obj->getDuration();
		} else {
			if (node->myStar())
				out += node->myStar()->readName();
			else if (node->getType() == -1)
				out += "send";
			else
				out += "recv";
			out += " (";
		}
		out += obj->getDuration();
		out += ")\t";
		if (form >= 2) {
			out += "\n";
			form = 0;
		}
		form++;
	}

	// statistics.
	out += "\n** total idle time is ";
	out += sumIdle;
	out += "\n";

	return out;
}

// write Gantt chart.
int UniProcessor :: writeGantt(UserOutput& o) {

	int total = 0;
	int start = 0;
	int fin = 0;

	ProcessorIter iter(*this);
	NodeSchedule* obj;

	// we must write lines in reverse order: revList is
	// used for order reversal
	SequentialList revList;
	while ((obj = iter++) != 0) {
		ParNode* node = (ParNode*) obj->getNode();
		if (!node->getType()) { // Not a comm star
			total += obj->getDuration();
			fin = start + obj->getDuration();

			char tmpbuf[160];
			const char* starName = node->myStar()->readName();
			sprintf (tmpbuf, "\t%s {%d} (%d %d)\n",
                                         starName,
                                         node->invocationNumber(),
                                         start, fin);
			revList.tup(savestring(tmpbuf));
			start = fin;
		} else { // A Communication node or idle node
			start += obj->getDuration();
		}
	}
	// go through revList and write elements in reverse
	ListIter nextl(revList);
	char* p;
	while ((p = (char*)nextl++) != 0) {
		o << p;
		LOG_DEL; delete p;
	}
	o << "$end proc$\n";
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
int UniProcessor :: filledInIdleSlot(ParNode* node, int start) {

	int load = node->getExTime();
	int curTime = getAvailTime();
	NodeSchedule* obj = (NodeSchedule*) myTail();
	ParNode* temp;

	while (obj) {
		curTime -= obj->getDuration();
		temp = obj->getNode();
		if (obj->isIdleTime() == 0) { // regular or visible comm node
			if (curTime < start + load) {
				return -1;
			} 
		} else { // idle node or comm node which is hidden.
			if (obj->getDuration() > load) {
				if (curTime < start) curTime = start;
				return curTime;
			} 
		}
		obj = (NodeSchedule*) obj->previousLink();
	}
	return -1;
}

// schedule in the middle (during an idle slot)
// return the completion time if succeeded, else "FALSE"
int UniProcessor :: schedInMiddle(ParNode* pd, int when, int leng) {

	int curTime = getAvailTime();
	NodeSchedule* obj = (NodeSchedule*) myTail();
	ParNode* temp = obj->getNode();

	while (obj) {
		curTime -= obj->getDuration();
		temp = obj->getNode();
		// check the current node should be an idle node.
		if (obj->isIdleTime() == 0) {  // if regular node or comm. node.
			if (curTime < when + leng) {
				return FALSE;
			}
		} else if (obj->getDuration() >= leng) {
			if (curTime > when) when = curTime;
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
	return FALSE;
}

// Assign nodes into the processor
void UniProcessor :: appendNode(ParNode* n, int val) {
	NodeSchedule* ns = getFree();
	ns->setMembers(n, val);
	appendLink(ns);
	curSchedule = ns;
}

			/////////////////
			///  addNode  ///
			/////////////////
// Adds the node to the schedule sometime after start
// It looks through the gaps and tries to fill them in if it can.

void UniProcessor::addNode(ParNode *node, int start) {

	// If earliestStart is >= availTime, then we append the node
	int d = node->getExTime();
	if (start < availTime) {
		// start < availTime so check gaps
		int newStart = filledInIdleSlot(node,start);
		if (newStart >= 0) {
			if (!schedInMiddle(node,newStart,d))
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
		ParNode* node = (ParNode*) obj->getNode();
		if (obj->isIdleTime()) {	// idle node
			idleTime += obj->getDuration();
		} else {
			return idleTime;
		}
	}
	return 0;
}

		/////////////////////////////////////
		//    down-load the code
		/////////////////////////////////////

void UniProcessor :: run() {

	// iterator for the scheduled node
	ProcessorIter schedIter(*this);
	NodeSchedule* obj;

	while((obj = schedIter++) != 0) {
		ParNode* node = (ParNode*) obj->getNode();
		if (obj->isIdleTime() == 0) {	// check idle node
			CGStar* s = node->myStar();
			if (!s) continue;
			if (s->isItWormhole()) {
				CGWormhole* worm = s->myWormhole();
				int index = obj->getIndex();
				worm->downLoadCode(index);
			} else {
				s->fire();
			}
		}
	}
}

// destructor
UniProcessor :: ~UniProcessor() {
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
		temp->resetMembers();
		freeNodeSched = temp->nextFree;
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