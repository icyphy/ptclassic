static const char file_id[] = "ParProcessors.cc";

/*****************************************************************
Version identification:
@(#)ParProcessors.cc	1.28	12/08/97

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

Programmer: Soonhoi Ha

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ParProcessors.h"
#include "ParGraph.h"
#include "StringList.h"
#include "SimControl.h"
#include "GalIter.h"

ParProcessors :: ParProcessors(int pNum, MultiTarget* t) :
moveStars(0), mtarget(t) {
	numProcs = pNum;
	pIndex.create(pNum);
	SCommNodes.initialize();
	commCount = 0;
	LOG_NEW; schedules = new UniProcessor[pNum];
	candidate.create(pNum);
}

ParProcessors :: ~ParProcessors() {
	removeCommNodes();
	LOG_DEL; delete [] schedules;
}

void ParProcessors :: initialize()
{
        int i;
	for (i = 0; i < numProcs; i++) {
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

UniProcessor* ParProcessors :: getProc(int num) {
	return &(schedules[num]); 
}

// map targets for each processing element
// If the argument is non-zero pointer, the array contains the mapping
// information.
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

IntArray* ParProcessors :: candidateProcs(DataFlowStar* s) {
        // clear the array.
        int k = 0;
        int flag = 0;
        int temp = -1;
 
        // heterogeneous case
        int hetero = mtarget->isHetero();
 
        // Scan the processors. By default, include only one
        // unused processor and all used processors which satisfy
        // resource constraints.
        for (int i = 0; i < numProcs; i++) {
                UniProcessor* uni = &(schedules[i]);
                CGTarget* t = uni->target();
                if (s && t && !t->support(s)) continue;
                else if (s && t && !mtarget->childHasResources(*s, i)) 
			continue;
                else if (uni->getAvailTime() || hetero) {
                        candidate[k] = i;
                        k++;
                } else if (!flag) {
                        temp = i;
                        flag = 1;
                }
        }
        if (temp >= 0) { candidate[k] = temp; k++; }
        candidate.truncate(k);
        if (!k) return 0;
	else return &candidate;
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
			if (!scheduleParNode(node)) return -1;
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

int ParProcessors :: scheduleParNode(ParNode* /*node*/) {
	Error :: abortRun("this scheduler does not support parallel tasks.",
		"\n Try macroScheduler instead.");
	return FALSE;
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
	// processors.  Insert send/receive nodes on these arcs
	EGIter iter(*graph);
	ParNode* pg;
	while ((pg = (ParNode*) iter++) != 0) {

           // Current send node
	   ParNode* snode = NULL;

	   // Current receive node
	   ParNode* rnode = NULL;

	   // last Porthole considered
	   const PortHole* lastP = NULL;

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
				 (g->aliasedPort() != lastP)) {
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

					lastP = g->aliasedPort();
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

int ParProcessors::replaceCommStar(DataFlowStar& newStar,
				   DataFlowStar& oldStar) {
    EGNodeListIter iter(SCommNodes);
    ParNode* pn;
    int replaceFlag = FALSE;
    while ((pn = (ParNode*) iter++) != 0) {
	if (pn->getCopyStar() == &oldStar) {
	    pn->replaceCopyStar(newStar);
	    replaceFlag = TRUE;
	}
    }
    return replaceFlag;
}

/*****************************************************************
		DISPLAY 
 ****************************************************************/

StringList ParProcessors :: display (NamedObj* galaxy) {

    int makespan = getMakespan();
    int sum = 0;	// Sum of idle time.

    StringList out;

    // title and number of processors.
    out << "  { galaxy " << galaxy->fullName() << " }\n"
	<< "  { numberOfProcessors " << numProcs << " }\n";

    // Per each processor.
    for (int i = 0; i < numProcs; i++) {
	out << "  { processor " << i << " "
	    << getProc(i)->display(makespan) << "  }\n";
	sum += getProc(i)->getSumIdle();
    }

    double util = 100*(1 - sum / (double(numProcs)*double(makespan)));

    // Print out the schedule statistics.
    out << "  { makespan " << makespan << " }\n";
    out << "  { idletime " << sum << " }\n";
    out << "  { utilization " << util << " }\n";

    return out;
}

/*****************************************************************
		sort the processors 
 ****************************************************************/

// sort the processor ids with available times.

void ParProcessors :: sortWithAvailTime(int guard) {
	int i;
	// initialize indices

	for (i = 0; i < numProcs; i++) {
		pIndex[i] = i;
	}

	// insertion sort
	// All idle processors are appended after the processors
	// that are available at "guard" time and before the processors
	// busy.

       	for (i = 1; i < numProcs; i++) {
               	int j = i;
		int temp1 = pIndex[i];
		UniProcessor* up = getProc(temp1);
               	int x1 = up->getAvailTime();
		int temp2 = pIndex[j-1];
		int x2 = getProc(temp2)->getAvailTime();
		if (x1 == 0) {	// unused processors
               		while (j > 0 && x2 > guard) {
		 		pIndex[j] = pIndex[j-1];
                       		j--;
				if (j > 0) {
					temp2 = pIndex[j-1];
					x2 = getProc(temp2)->getAvailTime();
               			}
			}
               	} else if (x1 > guard) { // busy processors
			while (j > 0 && x1 < x2 && x2 > guard) {
				pIndex[j] = pIndex[j-1];
                        	j--;
				if (j > 0) {
					temp2 = pIndex[j-1];
					x2 = getProc(temp2)->getAvailTime();
                		}
			}
             	
		} else {	// available processors
			while (j > 0 && (x1 < x2 || x2 == 0)) {
                       		pIndex[j] = pIndex[j-1];
                       		j--;
				if (j > 0) {
					temp2 = pIndex[j-1];
					x2 = getProc(temp2)->getAvailTime();
                		}
			}
		}
                pIndex[j] = temp1;
        }
}

/*****************************************************************
		create sub Galaxies for each processor
 ****************************************************************/

void ParProcessors :: createSubGals() {
        int i;
	// processor index set.
	for (i = 0; i < numProcs; i++) 
		getProc(i)->index = i;

	// create sub-Universes
	for (i = 0; i < numProcs; i++) {
		getProc(i)->createSubGal();
		if (SimControl::haltRequested()) break;
	}

	// We may leak memory here if SimControl::haltRequested()
	// We could be more careful - if there is an error in building
	// the subgalaxies - empty them and don't empty the original
	// galaxy.  FIXME
	
	// empty the galaxy if we are moving the stars
	if (moveStars) {
	    // must remove all the stars from the galaxies - because
	    // they have been MOVED.  We can't call empty on the
	    // Galaxy or we'll leak the Galaxy memory
	    GalStarIter stars(*mtarget->galaxy());
	    Star* star;
	    while ((star = stars++) != NULL)
		stars.remove();
	    mtarget->requestReset();
	}
}

StringList ParProcessors :: displaySubUnivs() {

	StringList out;
	for (int i = 0; i < numProcs; i++) {
		out << "  { processor " << i << "\n";
		out << getProc(i)->displaySubUniv();
		out << "  }\n";
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

