static const char file_id[] = "CGCaseScheduler.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCaseScheduler.h"
#include "Galaxy.h"
#include "DataStruct.h"
#include "CGMacroCluster.h"
#include "distributions.h"
#include "Domain.h"
#include <math.h>

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

 Programmer:  Soonhoi Ha

 Methods for CGCaseScheduler class.
**************************************************************************/

// the old g++ requires the obsolete "delete [size] ptr" form
// and new compilers use "delete [] ptr".  Handle with an ifdef.

#if defined(__GNUG__) && __GNUG__ == 1
#define SAVENUM saveNum
#define PREVNUM prevNum
#else
#define SAVENUM /* nothing */
#define PREVNUM /* nothing */
#endif

CGCaseScheduler :: ~CGCaseScheduler() {
	LOG_DEL; delete [SAVENUM] pis;
	LOG_DEL; delete [SAVENUM] touched;
	LOG_DEL; delete [SAVENUM] arcProfile;
	LOG_DEL; delete [SAVENUM] deltas;
	LOG_DEL; delete [PREVNUM] taus;
}

int CGCaseScheduler :: getStatistics() {

	// read the type of the distribution
	if (!dist.setType(refGal, mtarget)) return FALSE;

	// check the type of the distribution
	if (dist.readType() != GEOMETRIC && dist.readType() != GENERAL) {
		Error :: abortRun("Case has an improper distribution type");
		return FALSE;
	}

	// set the paramter information
	if (!dist.myParam()->setParams(refGal, mtarget)) {
		Error :: abortRun("Case: parameter reading error!");
		return FALSE; 
	}
	if (logstrm) {
		*logstrm << "Case construct with the following statistics:\n";
		*logstrm << dist.myParam()->printParams();
	}
	return TRUE;
}

int CGCaseScheduler :: examine(CGMacroClusterGal* galaxy) {
	checkNumArcs(galaxy);
	Case::clearSchedules();
        if (!Case::checkTopology(galaxy)) {
           Error::abortRun(*galaxy, 
		" has a wrong topology for Case construct");
           return FALSE;
        }
	return TRUE;
}

int CGCaseScheduler :: closerExamine() {

	// check whether there is only one galaxy on each arc
	for (int i = numArc - 2; i > 0; i--) {
		if (schedules[i].size() != 1) {
			Error::abortRun("Each arc wants a single galaxy!");
			return FALSE;
		}
	}
	return TRUE;
}

void CGCaseScheduler :: initMembers() {

	int n = numArc - 2;
	if (!saveNum) {	// execute first.
		LOG_NEW; arcProfile = (Profile**) new Profile* [n];
		LOG_NEW; pis = new double[n];
		LOG_NEW; touched = new int[n];
	} else if (saveNum != n) {
		LOG_DEL; delete [SAVENUM] arcProfile;
		LOG_DEL; delete [SAVENUM] pis;
		LOG_DEL; delete [SAVENUM] touched;
		LOG_NEW; arcProfile = (Profile**) new Profile* [n];
		LOG_NEW; pis = new double[n];
		LOG_NEW; touched = new int[n];
	}
	saveNum = n;

	// build the "deltas"
	LOG_DEL; delete [SAVENUM] deltas;
	LOG_NEW; deltas = new int [saveNum];

	// read pis.
	if (dist.readType() == GEOMETRIC) {
		if (n != 2) {
			Error::abortRun("Geometric distribution can be used \
for if-then-else only");
			return;
		}
		read2Pis();
	} else {
		if (n < 2) {
			Error::abortRun("Case type requires more than two \
arcs.");
			return;
		}
		readNPis();
	}
}

	///////////////////////
	//  Read Probabilities
	///////////////////////

void CGCaseScheduler :: read2Pis() {

	DistGeometric* geo = (DistGeometric*) dist.myParam();
	pis[1] = geo->getP();
	pis[0] = 1 - pis[1];
}

void CGCaseScheduler :: readNPis() {
	
	DistGeneral* gen = (DistGeneral*) dist.myParam();
	double tsum = 0;
 
	for (int i = 0; i < saveNum; i++) {
		double p = gen->getTable(i).value;
		pis[gen->getTable(i).index] = p;
		tsum += p;
	}
	if (tsum > 1) {
		Error::abortRun("inconsistent values for pis");
	}
}

	///////////////////////
	//  assumeExecTime
	///////////////////////

int CGCaseScheduler :: assumeExecTime() {

	double sum = 0;
	for (int i = 0; i < saveNum; i++) {
		ListIter bodyMacro(schedules[i+1]);
		CGMacroCluster* c = (CGMacroCluster*) bodyMacro++;
		sum += pis[i] * c->myExecTime();
	}

	return int(sum);
}

	///////////////////////
	//  adjustMembers
	///////////////////////

void CGCaseScheduler :: adjustMembers() {

	// adjust taus
	LOG_DEL; if (taus) delete [PREVNUM] taus;
	LOG_NEW; taus = new Taus[numProcs];
	prevNum = numProcs;

	// adjust arcProfile
	for (int i = 0; i < saveNum; i++) {
		ListIter bodyMacro(schedules[i+1]);
		CGMacroCluster* c = (CGMacroCluster*) bodyMacro++;
		arcProfile[i] = c->getProfile(0);
	}

}


	///////////////////////
	//  setProfile
	///////////////////////

int CGCaseScheduler :: setProfile(int num, int /*resWork*/, Profile* prof) {

	// adjust Members
	adjustMembers();

	// point to the right set of profiles.
	int check = FALSE;
	int noSort = FALSE;
	for (int i = 0; i < saveNum; i++) {
		arcProfile[i] = arcProfile[i] + num - 1;

		// determine whether to sort the profile or not.
		if (!mtarget->assignManually())
			arcProfile[i]->sortWithFinishTime(1);
		else
			noSort = TRUE;

		if (arcProfile[i]->getEffP() >= num) check = TRUE;
	}
	if (!check) return FALSE;
	
	// Initially set up taus with the maximum profiles.
	initTaus(num);
	initDeltas(num);

	// monitor the following flag. If it becomes zero for a scan of
	// all branches, we are done.
	int flagChanged = saveNum;
	int indexArc = 0;

	while (flagChanged) {
		flagChanged--;

		if (decreaseTaus(num, indexArc))
			flagChanged = saveNum;
		indexArc++;
		if (indexArc >= saveNum) indexArc = 0;
	}

	// write the final profile : startTime;
	calcStartTimes(num, prof);

	// write the final profile : finishTime..
	prof->setEffP(num);
	double tCost = 0;

	for (int j = 0; j < num; j++) {
		prof->setFinishTime(j, taus[j].val);
		tCost += prof->getFinishTime(j) - prof->getStartTime(j);
	}

	// sort the profile with its startTimes
	if (!noSort) prof->sortWithStartTime(1);

	// Add control and synchronization codes ahead of the profile
	tCost += addControlCodes(num, prof);
		
	// calculate the total cost...
	double tempSum = 0;
	for (j = 0; j < saveNum; j++) {
		int max = 0;
		if (max < deltas[j]) max = deltas[j];
		tempSum += pis[j] * double(max);
	}
	tCost += numProcs * tempSum;

	// summary the profile.
	prof->setTotalCost(tCost);
	prof->summary();
	return TRUE;
}
	

	///////////////////////
	//  calcStartTimes
	///////////////////////

void CGCaseScheduler :: calcStartTimes(int num, Profile* prof) {

	for (int i = 0; i < num; i++) {
		int min = arcProfile[0]->getStartTime(i);
		for (int j = 1; j < saveNum; j++) {
			if (min > arcProfile[j]->getStartTime(i))
				min = arcProfile[j]->getStartTime(i);
		}
		prof->setStartTime(i, min);	
	}
}

	///////////////////////
	//  addControlCodes
	///////////////////////

int CGCaseScheduler :: addControlCodes(int num, Profile* prof) {

	int ct = myControlTime();
	int st = mySyncTime();
	int temp = ct+st;

	prof->setFinishTime(0, prof->getFinishTime(0) + temp);

	for (int i = 1; i < num; i++) {
		prof->setStartTime(i, prof->getStartTime(i) + ct);
		prof->setFinishTime(i, prof->getFinishTime(i) + temp);
	}

	return (temp + (num - 1) * st);
}

	///////////////////////
	//  initTaus
	///////////////////////

// Initially, "taus" are set to the maximum schedule lengths among the
// profiles of all branches.  

void CGCaseScheduler :: initTaus(int num) {

	for (int index = 0; index < num; index++) {
		int max = 0;
		for (int i = 0; i < saveNum; i++) {
			if (index < arcProfile[i]->getEffP()) {
				int t = arcProfile[i]->getFinishTime(index);
				if (max < t) max = t;
			}
		}
		taus[index].val = max;
	}
}

	///////////////////////
	//  initDeltas
	///////////////////////

void CGCaseScheduler :: initDeltas(int num) {

	for (int i = 0; i < saveNum; i++) {
		int max = arcProfile[i]->getFinishTime(0) - taus[0].val;
		int j = 1;

		// initially "max" can not be greater than 0.
		while (max < 0 && j < num) {
			int t = arcProfile[i]->getFinishTime(j) - taus[j].val;
			if (t > max) max = t;
			j++;
		}
		deltas[i] = max;
	}
}

	///////////////////////
	//  decreaseTaus
	///////////////////////

// The scheduling routine decreases "taus" from the maximum to a certain
// value as long as the total cost decreases by doing that.
// When decreasing "taus", we have to update "deltas", too.

int CGCaseScheduler :: decreaseTaus(int num, int indexArc) {

	int changed = FALSE;

	if (deltas[indexArc] < 0) return FALSE;

	// if pis[indexArc] is large enough, return immediately.
	if (numProcs*pis[indexArc] - num >= 0) return FALSE;

	// Find the index set "theta" and sum of probability over "phi" set.
	// "theta"[indexArc] : this index determines the deltas[indexArc]
	// "phi"[indexArc] : index set whose theta shares a non-empty
	//		     indexs with theta[indexArc].

	int magTheta = 0;	// cardinality of "theta" index set.
	for (int i = 0; i < num; i++) {
		if (arcProfile[indexArc]->getFinishTime(i) - taus[i].val
			>= deltas[indexArc])
		magTheta++;
	}

   while (numProcs*pis[indexArc] - magTheta < 0) {

	// reset touched[]
	for (i = 0; i < saveNum; i++)
		touched[i] = 0;

	int blocked = FALSE;
	double sumPi = pis[indexArc];
	int minDel = taus[0].val;

	for (i = 0; i < num; i++) {

	   taus[i].flag = FALSE;
	   if (arcProfile[indexArc]->getFinishTime(i) 
			- taus[i].val >= deltas[indexArc]) {
		double tempi = sumPi;
		int tempMin = minDel;
		for (int j = 0; j < saveNum; j++) {
		   if (j != indexArc) {
			int d = arcProfile[j]->getFinishTime(i) - taus[i].val;
			if (d >= deltas[j] && d >= 0) {
				if (!touched[j]) {
					tempi += pis[j];
					touched[j] = -1;
				}
			} else {
				if (tempMin > deltas[j] - d) 
					tempMin = deltas[j] - d;
			}
		   }
		}
		if ((numProcs * tempi - magTheta < 0) && (tempMin > 0)) {
			sumPi = tempi;
			minDel = tempMin;
			taus[i].flag = TRUE;
			for (int k = 0; k < saveNum; k++) {
				if (touched[k] == -1) touched[k] = 1;
			}
		} else {
			magTheta--;
			for (int k = 0; k < saveNum; k++) {
				if (touched[k] == -1) touched[k] = 0;
			}
			// check  the delta cost again!
			if (numProcs * sumPi - magTheta >= 0) {
				blocked = TRUE;
				break;
			}
		}
	   }
	}

	// if blocked, continue.
	if (blocked) continue;

	// renew taus & deltas...
	for (i = 0; i < num; i++) {
		if (taus[i].flag) {
			changed = TRUE;
			// renew taus
			taus[i].val -= minDel;
			// renew Deltas
			for (int j = 0; j < saveNum; j++) {
				int d = arcProfile[j]->getFinishTime(i) -
					taus[i].val;
				if (d > deltas[j]) deltas[j] = d;
			}
		}
	}
   }
   return changed;
}

        ///////////////////////
        //  download the code
        ///////////////////////

int CGCaseScheduler :: downLoadCode(CGStar* macroS,
				    int invoc, int pix, int pId) {

	CGTarget* t = (CGTarget*) mtarget->child(pId);

	// set up the communication star for synchronization
	CGStar* s = 0;
	Geodesic* gd = 0;
	if (pix) {
		// receiving star.
		s = syncCommStar(syncReceiveStars, invoc, pix, 1);
		if (!t->incrementalAdd(s)) return FALSE;
	} else {
		// find Geodesic of the outside macro-actor connection to the
		// control PortHole
		gd = findGeo(macroS, "control");

		if (!addSendCodes(gd, invoc, t)) return FALSE;
	}
		
	mtarget->ddf()->startCode_Case(s, gd, t);
	if (haltRequested()) return FALSE;

	// inside macro-actor
	for (int i = 0; i < saveNum; i++) {
		ListIter bodyMacro(schedules[i+1]);
		CGMacroCluster* macroActor = (CGMacroCluster*) bodyMacro++;
		Geodesic* saveG1 = 0, *saveG2 = 0;
		CGPortHole* inp = 0;
		CGPortHole* outp = 0;
		if (!pix) {
			// switch geodesic of the macro boundary of 
			// the inside. 
			Geodesic* gd1 = findGeo(macroS, "Case");
			Geodesic* gd2 = findGeo(macroS, "EndCase");
			inp = boundaryPort(macroActor, 0);
			if (inp) {
				saveG1 = inp->geo();
				inp->switchGeo(gd1);
			}
			outp = boundaryPort(macroActor, 1);
			if (outp) {
				saveG2 = outp->geo();
				outp->switchGeo(gd2);
			}
		}
		downLoadBodyCode(macroActor->asSpecialBag(), pix, t);

		if (inp) inp->revertGeo(saveG1);
		if (outp) outp->revertGeo(saveG2);

		mtarget->ddf()->middleCode_Case(i, t);
		if (haltRequested()) return FALSE;
	}
	mtarget->ddf()->endCode_Case(t);
	if (haltRequested()) return FALSE;
	return TRUE;
}

// virtual methods --- default

int CGCaseScheduler :: myControlTime()	{ return 2; }
int CGCaseScheduler :: mySyncTime()	{ return 2; }

