static const char file_id[] = "ParProcessors.cc";

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

#include "ParProcessors.h"
#include "ParGraph.h"
#include "StringList.h"

ParProcessors :: ParProcessors(int pNum, MultiTarget* t) : mtarget(t) {
	numProcs = pNum;
	pIndex.create(pNum);
	pId.create(pNum); 
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
		pId[i] = i;
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
void ParProcessors :: mapTargets() {
	for (int i = 0; i < numProcs; i++) {
		getProc(i)->targetPtr = (CGTarget*) mtarget->child(i);
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
	findCommNodes(graph, readyNodes);

	while (readyNodes.size() > 0) {
		ParNode* node = (ParNode*) readyNodes.takeFromFront();
		UniProcessor* p = getProc(node->whichProc());

		// compute the earliest schedule time
		int start = ancestorsFinish(node);
		
		// If node is a comm node, call topology dependent section
		// to see if there are any constraints
		if (node->getType() < 0) {
			p->scheduleCommNode(node, start);
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

			///////////////////////
			///  findCommNodes  ///
			///////////////////////
// Makes send and receive ParNodes for each interprocessor communication.
// Want to splice these into tempAncs and tempDescs for list scheduling.

void ParProcessors::findCommNodes(ParGraph* graph, EGNodeList& /*readyNodes*/)
{
	// Make sure the list of communication nodes is clear
	removeCommNodes();
	int hidden = FALSE;

	EGIter iter(*graph);
	ParNode *pg;
	while ((pg = (ParNode*) iter++) != 0) {
	   hidden = FALSE;
	   EGGateLinkIter giter(pg->descendants);
	   EGGate* g;
	   for (int i = 2; i > 0; i--) {	// loop twice
		while ((g = giter++) != 0) {
			if (hidden && g->isItInput()) continue;
			ParNode* desc = (ParNode*)(g->farEndNode());
			int from = pg->whichProc();
			int to = desc->whichProc();
			if (from != to) {
				int num = g->samples();

				// make comm nodes.
				// 1. compute communication times.
				int sTime = mtarget->commTime(from,to,num,0);
				int rTime = mtarget->commTime(from,to,num,1);

				// 2. make send and receive nodes
				ParNode* snode = createCommNode(-1);
				snode->assignProc(from);
				snode->setOrigin(g);
				if (hidden) {
					// can be done later.
					snode->assignSL(1);	
				} else {
					snode->assignSL(pg->getLevel() + 1);
				}
				snode->setExTime(sTime);
				
				ParNode* rnode = createCommNode(-2);
				rnode->assignProc(to);
				rnode->setOrigin(g->farGate()); 
				if (hidden) {
					// can be done later
					rnode->assignSL(1);
				} else {
					rnode->assignSL(desc->getLevel());
				}
				rnode->setExTime(rTime);

				// 3. insert them into the graph.
				pg->connectedTo(snode);
				if (!hidden) rnode->connectedTo(desc);
				snode->connectedTo(rnode);

				snode->setPartner(rnode);
				SCommNodes.insert(snode);
				SCommNodes.insert(rnode);
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

void ParProcessors :: sortWithFinishTime() {

	// insertion sort
        for (int i = 1; i < numProcs; i++) {
                int j = i - 1;
		UniProcessor* up = getProc(i);
                int x = up->getAvailTime();
                int temp = pId[i];
                while (j >= 0) {
			if (x > getProc(j)->getAvailTime()) break;
                        pId[j+1] = pId[j];
                        j--;
                }
                j++;
                pId[j] = temp;
        }
}

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
		mtarget->addProcessorCode(i, (const char*) foo);
	}
}		
