/**************************************************************************
Version identification:
$Id$
 
Copyright (c) 1995 Seoul National University
All rights reserved.
 
 Programmer:  S. Ha
 
**************************************************************************/
static const char file_id[] = "CGDDFCluster.cc";
 
#ifdef __GNUG__
#pragma implementation
#endif
 
#include "CGDDFCluster.h"
#include "GalIter.h"
#include "Geodesic.h"
#include "Plasma.h"
#include "Target.h"
#include "CGUtilities.h"
#include "CGCaseScheduler.h"
#include "CGDoWhileScheduler.h"
#include "CGForScheduler.h"
#include "CGRecurScheduler.h"
#include "pt_fstream.h"
#include "Error.h"
 
CGMacroClusterBag* CGDDFClusterGal :: createGalBag(Galaxy* g) {
	// check whether the galaxy is DDF type or not
	CGMacroClusterBag* curBag;
	int flag = 0;
	DFGalStarIter nextStar(*g);
        DataFlowStar* s;
        while ((s = nextStar++) != 0) {
		DFStarPortIter nextp(*s);
		DFPortHole* p;
		while ((p = nextp++) != 0) {
			if (p->isVarying()) {	// DDF star
				flag = 1;
				break;
			}
		}
		if (flag) break;
	}
	if (flag) {
                LOG_NEW; curBag = new CGDDFClusterBag(g);
	} else {
                LOG_NEW; curBag = new CGMacroClusterBag(g);
	}
	addBlock(curBag->setBlock(genBagName(), this));
	curBag->resetId();      // make it look like an atomic cluster
	if (logstrm) {
		*logstrm << curBag->name() << "(" <<
		g->className() << ")\n";
	}
        return curBag;
}


// cluster inside galaxy clusters
int CGDDFClusterBag :: clusterMyGal() {
	CGMacroClusterGalIter nextClust(*gal);
	CGMacroCluster* c;

	while ((c = nextClust++) != 0) {
		CGMacroClusterBag* cbag = (CGMacroClusterBag*) c->asSpecialBag();
		if (cbag) {
			if (!cbag->clusterMyGal()) return FALSE;
		}
	}
	return TRUE;
}

int CGDDFClusterBag :: prepareBag(MultiTarget* t, ostream* l) {
	mtarget = t;
	if (conSched) return TRUE;

	// create scheduler and profile
	StringState* st = (StringState*) t->galParam(pGal, "constructType");
	if (!st) {
		Error::abortRun(*this, "undefined construct type");
		return FALSE;
	}
	char* cType = makeLower((const char*) *st);
 
	switch (*cType) {
		case 'c': LOG_NEW; conSched = new CGCaseScheduler(t,l);
			break;
		case 'd': LOG_NEW; conSched = new CGDoWhileScheduler(t,l);
			break;
		case 'f': LOG_NEW; conSched = new CGForScheduler(t,l);
			break;
		case 'r': LOG_NEW; conSched = new CGRecurScheduler(t,l);
			break;
		default:
			Error::abortRun(*this, "unknown construct type");
			return FALSE;
	}
	LOG_DEL; delete cType;
	conSched->setGalaxy(*gal);
	conSched->setOwner(this);
	conSched->setRefGal(pGal);

	// initialize a private member
	IntState* fn = (IntState*) t->galParam(pGal, "fixedNum");
	if (fn) fixNum = int(*fn);
	else fixNum = 0;

	// profile creation
	int numProcs = t->nProcs();
	LOG_NEW; profile = new Profile[numProcs];
	for (int i = 0; i < numProcs; i++) {
		profile[i].create(i+1);
		profile[i].createAssignArray(reps());
	}

	// adjust sample rates
	CGClustPortIter nextPort(*this);
	CGClustPort* p;
	while ((p = nextPort++) != 0 ) {
		if (p->numIO() == 0) p->setNumXfer(1);
	}
	return recurPrepareBag(t,l);
}

// make parallel schedule of CGDDF constructs.
int CGDDFClusterBag::parallelSchedule(MultiTarget* t) {
	if (!conSched->getStatistics()) return FALSE;
	
	// scheduler setup
	conSched->setup();
	if (Scheduler :: haltRequested()) return FALSE;

	return TRUE;
}

int CGDDFClusterBag :: myExecTime() {
	return conSched->avgExecTime();
}

int CGDDFClusterBag :: computeProfile(int nP, int rW, IntArray* avail) {
	numProcs = nP;
	conSched->setProcs(nP);
	conSched->copyProfile(profile+nP-1);

	// find optimal profile
	if (fixNum) {
		conSched->fixProfile(fixNum, rW, avail);
		optNum = fixNum;
	} else {
		optNum = conSched->calcProfile(rW, avail);
	}
	return optNum;
}

Profile* CGDDFClusterBag :: manualSchedule(int count) {
	// collect processor Ids of the component stars	
	int sz = mtarget->nProcs();
	IntArray procId;
	procId.create(sz);
	for (int i = sz - 1; i >= 0; i--) {
		procId[i] = -1;
	}
	int nP = examineProcIds(procId);
	if (nP < 0) return 0;
	optNum = nP;
	numProcs = optNum;

	// fill up processor assignment information in the profile
	Profile* pf = profile + nP - 1;
	for (i = 1; i <= count; i++) {
		for (int k = 0; k < nP; k++) {
			pf->assign(i,k,procId[k]);
		}
	}

	parallelSchedule(mtarget);
	setProcId(procId[0]);

	// schedule the inside clusters
	CGMacroClusterGalIter nextClust(*gal);
	CGMacroCluster* c;

	while ((c = nextClust++) != 0) {
		CGMacroClusterBag* cbag = (CGMacroClusterBag*) c->asSpecialBag();
		if (cbag) {
			// first change the procIds of atom clusters
			CGMacroClusterBagIter nextC(*cbag);
			CGMacroCluster* ac;
			while ((ac = nextC++) != 0) {
				if (ac->asSpecialBag()) continue;
				for (int k = 0; k < nP; k++) 
					if (procId[k] == ac->getProcId()) 
						break;
				if (k >= nP) k = -1;
				ac->setProcId(k);
			}

			MacroParSched* s = cbag->parSched();
			if (!mtarget->reorderChildren(pf->assignArray(1)))
				return 0;
			s->setUpProcs(nP);
			s->mapTargets();
			s->mainSchedule();
			if (Scheduler :: haltRequested()) return FALSE;
			if (!s->createSubGals(*cbag->clusterGal())) 
				return FALSE;
			if (!s->finalSchedule()) return FALSE;
			mtarget->restoreChildOrder();
		}
	}
	conSched->setOptNum(optNum);
	conSched->buildSyncLinks(pf);
	conSched->buildDataSyncs(pf);
	return pf;
}
	
int CGDDFClusterBag :: finalSchedule(int numProcs) {
	Profile* pf = profile + numProcs - 1;
	IntArray* a = pf->assignArray(1);
 
	CGMacroClusterGalIter nextClust(*gal);
	CGMacroCluster* c;
	while ((c = nextClust++) != 0) {
		CGMacroClusterBag* cbag = c->asSpecialBag();
		if (!cbag) continue;
	
		Profile* inprof = profile + optNum - 1;
		inprof->sortWithFinishTime(1);

		// fill up the processor assignment array for inner
		// schedule
		IntArray procMap;
		int sz = inprof->getEffP();
		procMap.create(sz);
		for (int i = 0; i < sz; i++) {
			int flag = TRUE;
			for (int j = 0; j < sz; j++)
				if (inprof->getProcId(j) == i) {
					flag = FALSE; break;
				}
			if (flag) return FALSE;
			procMap[i] = a->elem(j);
		}
	
		MacroParSched* s = cbag->parSched();
		s->setUpProcs(optNum);
		if(!mtarget->reorderChildren(&procMap)) return FALSE;
		s->mapTargets();
		s->mainSchedule();
		if (!s->createSubGals(*cbag->clusterGal())) return FALSE;
		if (!s->finalSchedule()) return FALSE;
		mtarget->restoreChildOrder();
	}
	conSched->buildSyncLinks(pf);
	conSched->buildDataSyncs(pf);
	return TRUE;
}

	//////////////////////////////
	// DownLoadCode
	//////////////////////////////
 
void CGDDFClusterBag :: downLoadCode(CGStar* macroS, int invoc, int pix) {
	Profile* pf = profile + numProcs - 1;
	int pId = 0;
	if (invoc) pId = pf->assignedTo(invoc, pix);
	conSched->downLoadCode(macroS, invoc, pix, pId);
}
 
	//////////////////////////////
	// Display parallel schedules
	//////////////////////////////
 
StringList CGDDFClusterBag :: displaySchedule() {
 
	CGMacroClusterGalIter next(*gal);
	CGMacroCluster* s;
 
	StringList out;
	out += "\n(Inside a dynamic construct)";
	out += "\n\n";
 
	// find out macro actors
	while ((s = next++) != 0) {
		CGMacroClusterBag* cbag = s->asSpecialBag();
		if (cbag) {
			out += " \n<";
			out += cbag->realName();
			out += ">\n\n";
			out += cbag->displaySchedule();
		}
	}
	return out;
}        

CGDDFClusterBag :: ~CGDDFClusterBag() {
	LOG_DEL; delete conSched;
	conSched = 0;
}
