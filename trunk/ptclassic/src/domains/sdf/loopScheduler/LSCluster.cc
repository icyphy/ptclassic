static const char file_id[] = "LSCluster.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Soonhoi Ha, Based on Shuvra's work.
 Date of creation: 4/92

 LSCluster class
 '

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "LSCluster.h"
#include "LSGraph.h"
#include "SDFCluster.h"
#include "ClusterNodeList.h"
#include "Target.h"

SDFFiring :: ~SDFFiring() { LOG_DEL; delete next;}

// constructor
LSCluster :: LSCluster (LSGraph& g, ClusterNodeList* clist) {
	
	firing = 0;
	prev = 0;

	// check whether this cluster contains only one type of nodes
	if (!clist->second) {
		LOG_NEW; firing = new SDFFiring(clist->first, clist->size());
	} else {
	// determine the order of firings of two masters.
		determineOrder(g, clist);
	}
}

void LSCluster :: determineOrder(LSGraph& g, ClusterNodeList* clist) {

	ClusterNodeListIter firstType(*clist);
	LSNode* nodePar = firstType++;
	SDFStar* first = clist->first;

	ClusterNodeListIter secondType(*clist);
	LSNode* nodeSon = secondType++;
	SDFStar* second = clist->second;
	while (nodeSon->myMaster() == first) {
		nodeSon = secondType++;
	}
	
	do {
		int count = 0;
		int flag = FALSE;

		while ((nodePar->myMaster() == first) && (flag == FALSE)) {
			if (g.pathExists(*nodeSon, *nodePar)) {
				addFiring(first, count);
				flag = TRUE;
			} else {
				nodePar = firstType++;
				count++;
			}
		}
		if (!flag) { 
			addFiring(first, count);
			flag = TRUE;
			nodePar = 0;
		}
		count = 0;
		while (nodeSon && (flag == TRUE)) {
			if (nodePar && g.pathExists(*nodePar, *nodeSon)) {
				addFiring(second, count);
				flag = FALSE;
			} else {
				nodeSon = secondType++;
				count++;
			}
		}
		if (flag) {
			addFiring(second, count);
		}
	} while (nodeSon);
}		

void LSCluster :: addFiring(SDFStar* s, int cnt) {

	LOG_NEW; SDFFiring* newF = new SDFFiring(s, cnt);
	if (!firing) { 
		firing = newF;
		prev = newF; 
	} else {
		prev->next = newF;
		prev = newF;
	}
}

// run the cluster
int LSCluster :: run() {

	SDFFiring* f = firing;
	while (f) {
		for (int i = f->count; i > 0; i--) {
			if (!f->s->run()) return FALSE;
		}
		f = f->next;
	}
	return TRUE;
}
	
// indent by depth tabs.
static const char* tab_2(int depth) {
	// this fails for depth > 20, so:
	if (depth > 20) depth = 20;
	static const char *tabs = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	return (tabs + 20 - depth);
}

// Display schedule
StringList LSCluster :: displaySchedule(int depth) {

	StringList sch;
	SDFFiring* f = firing;
	while (f) {
		if (f->count > 1) {
			sch += tab_2(depth);
			sch += f->count;
			sch += "*{\n";
			depth++;
		}

		
		SDFBaseCluster* temp = (SDFBaseCluster*)(f->s);
		sch += temp->displaySchedule(depth);

		if (f->count > 1) {
			depth--;
			sch += tab_2(depth);
			sch += "}\n";
		}
		f = f->next;
	}
	return sch;
}
	
// generate Code
void LSCluster :: genCode(Target& t, int depth) {

	SDFFiring* f = firing;
	while (f) {
		if (f->count > 1) {
			t.beginIteration(f->count, depth);
			depth++;
		}
		SDFBaseCluster* temp = (SDFBaseCluster*)(f->s);

		temp->genCode(t, depth);

		if (f->count > 1) {
			depth--;
			t.endIteration(f->count, depth);
		}
		f = f->next;
	}
	return;
}

