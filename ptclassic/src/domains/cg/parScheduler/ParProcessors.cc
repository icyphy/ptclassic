static const char file_id[] = "ParProcessors.cc";

/*****************************************************************
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

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ParProcessors.h"
#include "ParGraph.h"
#include "StringList.h"
#include "SimControl.h"

ParProcessors :: ParProcessors(int pNum, MultiTarget* t) : mtarget(t) {
	numProcs = pNum;
	pIndex.create(pNum);
	SCommNodes.initialize();
	commCount = 0;
}

ParProcessors :: ~ParProcessors() {
	removeCommNodes();
}

void ParProcessors :: initialize()
{
	for (int i = 0; i < numProcs; i++) {
		pIndex[i] = i;
	}

	// reset the pattern of processor availability.
	// initialize parallel schedules.
	for (i = 0; i < numProcs; i++) {
		UniProcessor* p = getProc(i);
		p->setTarget(mtarget, this);
		p->initialize();
	}

	removeCommNodes();
	commCount = 0;
}

// remove comm. nodes
void ParProcessors :: removeCommNodes() {
	EGNode* en;
	while ((en = (EGNode*) SCommNodes.takeFromFront()) != 0) {
		LOG_DEL; delete en;
	}
	SCommNodes.initialize();
}

UniProcessor* ParProcessors :: getProc(int) { return 0; }

// map targets for each processing element
// If the argument is non-zero pointer, the array contains the mapping
// information.
void ParProcessors :: mapTargets(IntArray* a) {
	for (int i = 0; i < numProcs; i++) {
		int ix;
		if (a && (i < a->size())) ix = a->elem(i);
		else ix = i;
		getProc(i)->targetPtr = (CGTarget*) mtarget->child(ix);
	}
}

// calculate the makespan...
int ParProcessors :: getMakespan() {
	int max = 0;
	for (int i = 0; i < numProcs; i++) {
		int temp = getProc(i)->getAvailTime();
		if (temp > max) max = temp;
	}
	return max;
}

/*****************************************************************
		list scheduling
 ****************************************************************/

// copy node lists
static void copyNodes(EGNodeList& from, EGNodeList& to) {
	to.initialize();
	EGNodeListIter niter(from);
	EGNode* node;
	while ((node = niter++) != 0)
		to.append(node);
}

// find the earliest time when all ancestors are scheduled.
static int ancestorsFinish(ParNode* node) {
	int finish = 0;
	ParAncestorIter iter(node);
	ParNode *pg;
	while ((pg = iter++) != 0) {
		if (pg->getFinishTime() > finish)
			finish = pg->getFinishTime();
	}
	return finish;
}

			/////////////////////
			///  listSchedule ///
			/////////////////////
// The list scheduler which obtains the makespan.  It finds all the
//      instances of interprocessor communication, constructs
//      new ParNodes to represent them, and schedules them along
//      with the other ParNodes.
int ParProcessors :: listSchedule(ParGraph* graph) {

	// runnable nodes
	EGNodeList readyNodes;
	copyNodes(graph->runnableNodes, readyNodes);

	// reset the graph
	graph->replenish(0);

	// find communication nodes and insert them into the graph.
	findCommNodes(graph);

	while (readyNodes.size() > 0) {
		ParNode* node = (ParNode*) readyNodes.takeFromFront();
		UniProcessor* p = getProc(node->getProcId());

		// compute the earliest schedule time
		int start = ancestorsFinish(node);
		
		// If node is a comm node, call topology dependent section
		// to see if there are any constraints
		if (node->getType() < 0) {
			p->scheduleCommNode(node, start);
		} else if (node->amIBig()) {
			scheduleParNode(node);
		} else {
			// schedule a regular node
			p->addNode(node, start);
		}

		// check whether any descendant is runnable after this node.
		ParDescendantIter iter(node);
		ParNode* n;
		while ((n = iter++) != 0) {
			if (n->fireable())
				graph->sortedInsert(readyNodes, n, 1);
		}
	}
	return getMakespan();
}

			////////////////////////
			///  scheduleParNode ///
			////////////////////////

void ParProcessors :: scheduleParNode(ParNode* node) {
	Profile* pf = node->profile();
	int num =  pf->getEffP();
	int invocNum = node->invocationNumber();

	// Compare the processor availability and the startTime profile
	int tempIx;
	int tempMax, refMin = -1;
	for (int i = 0; i < num; i++) {
		if (pf->getStartTime(i) == 0) {
			tempIx = pf->assignedTo(invocNum,i);
			tempMax = getProc(tempIx)->getAvailTime();
			if ((refMin < 0) || (refMin < tempMax)) {
				refMin = tempMax;
			} 
		}
	}

	// compute the shift.
	int shift = 0;
	for (i = 0; i < num; i++) {
		tempIx = pf->assignedTo(invocNum,i);
		tempMax = getProc(tempIx)->getAvailTime() - refMin -
			  pf->getStartTime(i);
		if (tempMax > shift) shift = tempMax;
	}

	// schedule the node
	int saveFinish = 0;
	for (i = 0; i < num; i++) {
		tempIx = pf->assignedTo(invocNum,i);
		UniProcessor* proc = getProc(tempIx);
		int leng = pf->getFinishTime(i) - pf->getStartTime(i);
		int when = refMin + shift + pf->getStartTime(i);
		proc->schedAtEnd(node, when, leng);
		if (saveFinish < when + leng) saveFinish = when + leng;
	}
	node->setScheduledTime(refMin + shift);
	node->setFinishTime(saveFinish);
}

			///////////////////////
			///  findCommNodes  ///
			///////////////////////
// Makes send and receive ParNodes for each interprocessor communication.
// Want to splice these into tempAncs and tempDescs for list scheduling.

void ParProcessors::findCommNodes(ParGraph* graph)
{
	// Make sure the list of communication nodes is clear
	removeCommNodes();

	// Iterate over all nodes in the expanded graph.  Search for
	// arcs that connect nodes that are allocated to different
	// processors.  Insert send/receive nodes on these arcs, group the
	// arcs if they are going to the same processor.
	EGIter iter(*graph);
	ParNode* pg;
	while ((pg = (ParNode*) iter++) != 0) {

           // Current send node
	   ParNode* snode = NULL;

	   // Current receive node
	   ParNode* rnode = NULL;

	   // Last node to receive data
	   ParNode* lastDesc = NULL;

	   // Number of tokens transfered from ParNode 'pg'
	   int numXfer = 0;
	   
	   // We loop twice, first over the non-hidden EGGates, and the through the
	   // hidden EGGates.  Gates that are hidden have delays that are associated
	   // with the gate
	   int hidden = FALSE;
	   EGGateLinkIter giter(pg->descendants);
	   for (int i = 2; i > 0; i--) {
		EGGate* g;
		while (( g = giter++) != 0) {
			int num = g->samples();  // Number of tokens transfered over gate
			if (num == 0) continue;
			if (hidden && g->isItInput()) continue;
			ParNode* desc = (ParNode*)(g->farEndNode());
			int from = pg->getProcId();
			int to = desc->getProcId();
			if (from != to) {
				// make send/receive nodes if necessary.
				if ((snode==NULL) ||
				    (rnode->getProcId() != to) ||
				    (desc->myStar() != lastDesc->myStar())) {
					snode = createCommNode(-1);
					rnode = createCommNode(-2);
					numXfer = 0;
					snode->setProcId(from);
					rnode->setProcId(to);
					snode->setOrigin(g);
					rnode->setOrigin(g->farGate()); 
					if (hidden) {
						snode->assignSL(1);	
						rnode->assignSL(1);
					}
					else {
						snode->assignSL(pg->getLevel() + 1);
						rnode->assignSL(desc->getLevel());
					}
					
				        // insert the send/receive nodes into the graph.
					pg->connectedTo(snode);
					snode->connectedTo(rnode);
					snode->setPartner(rnode);
					SCommNodes.insert(snode);
					SCommNodes.insert(rnode);

					// save the destination node
					lastDesc = desc;
				}
				if (!hidden) rnode->connectedTo(desc);

				// compute tokens transfered by
				// send/receive invocation
			        numXfer += num;
				
				// compute communication times.
				int sTime = mtarget->commTime(from,to,numXfer,0);
				int rTime = mtarget->commTime(from,to,numXfer,1);
				snode->setExTime(sTime);
				rnode->setExTime(rTime);
			}
		}
		giter.reconnect(pg->hiddenGates);
		hidden = TRUE;
	   }
	}
	// Set the communcation node count for the schedule
	commCount = (int)(SCommNodes.size() / 2);
}

ParNode*  ParProcessors :: createCommNode(int i) {
	LOG_NEW; return new ParNode(i);
}

/*****************************************************************
		Match comm. nodes with given comm. star
 ****************************************************************/

ParNode* ParProcessors :: matchCommNodes(DataFlowStar* s, EGGate* g, PortHole* p) {
	// set the cloned star pointer of the comm. node
	EGNodeListIter iter(SCommNodes);
	ParNode* pn;
	ParNode* saven = 0;
	while ((pn = (ParNode*) iter++) != 0) {
		if (g) {
			if (pn->getOrigin() == g) {
				pn->setCopyStar(s,0);
				s->setMaster(pn);
				return pn;
			}
		} else {
			if (pn->getOrigin()->aliasedPort() == p) {
				pn->setCopyStar(s,0);
				saven = pn;
			}
		}
	}
	return saven;
}

/*****************************************************************
		DISPLAY 
 ****************************************************************/

StringList ParProcessors :: display (NamedObj* galaxy) {

	int makespan = getMakespan();
	int sum = 0;	// Sum of idle time.

	StringList out;

	// title and number of processors.
	out += "** Parallel schedule of \"";
	out += galaxy->fullName();
	out += "\" on ";
	out += numProcs;
	out += " processors.";
	out += "\n\n";

	// Per each processor.
	for (int i = 0; i < numProcs; i++) {
		out += "[PROCESSOR ";
		out += i;
		out += "]\n";
		out += getProc(i)->display(makespan);
		sum += getProc(i)->getSumIdle();
		out += "\n\n";
	}

	double util = 100*(1 - sum / (double(numProcs)*double(makespan)));

	// Print out the schedule statistics.
	out += "************* STATISTICS ******************";
	out += "\nMakespan is ................. ";
	out += makespan;
	out += "\nTotal Idle Time is .......... ";
	out += sum;
	out += "\nProcessor Utilization is .... ";
	out += util;
	out += " %\n\n";

	return out;
}

/*****************************************************************
		sort the processors 
 ****************************************************************/

// sort the processor ids with available times.
void ParProcessors :: sortWithAvailTime(int guard) {

	// initialize indices
	for (int i = 0; i < numProcs; i++) {
		pIndex[i] = i;
	}

	// insertion sort
	// All idle processors are appended after the processors
	// that are available at "guard" time and before the processors
	// busy.

        for (i = 1; i < numProcs; i++) {
                int j = i - 1;
		UniProcessor* up = getProc(i);
                int x = up->getAvailTime();
                int temp = pIndex[i];
		if (x == 0) {	// If the processor is idle.
                	while (j >= 0) {
			   	if (guard < getProc(j)->getAvailTime()) 
					break;
				pIndex[j+1] = pIndex[j];
                        	j--;
			}
                } else if (x > guard) {
			while (j >= 0) {
				if(x < getProc(j)->getAvailTime()) break;
                        	pIndex[j+1] = pIndex[j];
                        	j--;
                	}
		} else {
			int temp = getProc(j)->getAvailTime();
			while (j >= 0 && (x < temp || temp == 0)) {
                        	pIndex[j+1] = pIndex[j];
                        	j--;
				if (j >=0)
					temp = getProc(j)->getAvailTime();
                	}
		}
                j++;
                pIndex[j] = temp;
        }
}

/*****************************************************************
		create sub Galaxies for each processor
 ****************************************************************/

void ParProcessors :: createSubGals() {

	// processor index set.
	for (int i = 0; i < numProcs; i++) 
		getProc(i)->index = i;

	// create sub-Universes
	for (i = 0; i < numProcs; i++) {
		getProc(i)->createSubGal();
		if (SimControl::haltRequested()) return;
	}
}

StringList ParProcessors :: displaySubUnivs() {

	StringList out;
	for (int i = 0; i < numProcs; i++) {
		out += " << Processor #";
		out += i;
		out += " >> \n";
		out += getProc(i)->displaySubUniv();
		out += "\n";
	}
	return out;
}

/*****************************************************************
		generate code
 ****************************************************************/

void ParProcessors :: prepareCodeGen() {
	for (int i = 0; i < numProcs; i++) {
		getProc(i)->prepareCodeGen();
	}
}		

void ParProcessors :: generateCode() {
	for (int i = 0; i < numProcs; i++) {
		StringList& foo = getProc(i)->generateCode();
		if (SimControl::haltRequested()) return;
		mtarget->addProcessorCode(i, (const char*) foo);
	}
}		
