static const char file_id[] = "CGDoWhileScheduler.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGDoWhileScheduler.h"
#include "Galaxy.h"
#include "DataStruct.h"
#include "distributions.h"
#include "CodeBlock.h"
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

 Methods for CGDoWhileScheduler class.
**************************************************************************/

int CGDoWhileScheduler :: getStatistics() {

	// read the type of the distribution
	if (!dist.setType(refGal, mtarget)) return FALSE;

	// set the paramter information
	if (!dist.myParam()->setParams(refGal, mtarget)) {
		Error :: abortRun("Case: parameter reading error!");
		return FALSE; 
	}
	if (logstrm) {
		*logstrm << "DoWhile with the following statistics:\n";
		*logstrm << dist.myParam()->printParams();
	}
	return TRUE;
}

int CGDoWhileScheduler :: examine(CGMacroClusterGal* galaxy) {
        DoWhile::clearSchedules();
        if (!DoWhile::checkTopology(galaxy)) {
                Error::abortRun(*galaxy, 
			" has a wrong topology for DoWhile construct");
                return FALSE;
        }
	return TRUE;
}

int CGDoWhileScheduler :: closerExamine() {

	// check whether there is only one clusterBag or more.
        int num = 0;

        // check up the "schedules" list one by one.
        ListIter top(schedules[0]);
        CGMacroCluster* s;
        while ((s = (CGMacroCluster*) top++) != 0) {
                if (s->asSpecialBag()) {
                        num++;
                        if (num > 1) {
				Error::abortRun("DoWhile construct wants a single clusterBag!");
				return FALSE;
                        } else {
                                cbag = (CGMacroClusterBag*) s;
                                body = cbag->getProfile(0);
                        }
                } 
        }

	return TRUE;
}

        ///////////////////////
        //  assumeExecTime
        ///////////////////////

int CGDoWhileScheduler :: assumeExecTime() {

        // take the total work of the clusterBag
        int work = cbag->myExecTime();
        double avg = dist.myParam()->assumedValue();
	avg = avg * double(work);

        return int(avg);
}

	///////////////////////
	//  setProfile
	///////////////////////

int CGDoWhileScheduler :: setProfile(int num, int resWork,
			Profile* prof) {

	// compute the optimal number of assigned processors.
	Profile& p = body[num-1];
	
	// check the effective number of processors
	if (p.getEffP() < num) return FALSE;

	// compute the profile
	tau = p.getMakespan();
	double totalCost = calcCost(num, resWork);

	// record the profile
	int span = bestX*tau;
	for (int j = 0; j < num; j++) {
		prof->setStartTime(j, 0);
		prof->setFinishTime(j, span);
	}
	
	// add synchronization code at the end of each iteration cycle.
	totalCost += addSyncCodes(num, prof);

	prof->setEffP(num);
	prof->setTotalCost(totalCost);
	prof->summary();
	return TRUE;
}
	
        ///////////////////////
        //  addControlCodes
        ///////////////////////

int CGDoWhileScheduler :: addSyncCodes(int num, Profile* prof) {

        int st = mySyncTime() * bestX;

        for (int i = 0; i < num; i++) {
                prof->setFinishTime(i, prof->getFinishTime(i) + st);
	}
	
	return (st * num);
}

	///////////////////////
	//  calcCost
	///////////////////////

double CGDoWhileScheduler :: calcCost(int num, int resWork) {

	// calculate bestX based on the distribution of the
	// the iteration steps.
	switch (dist.readType()) {
		case GEOMETRIC:	return costInGeometric(num,resWork);
		case UNIFORM:	return costInUniform(num,resWork);
		case GENERAL:	return costInGeneral(num,resWork);
		default:   return FALSE;
	}
}
		
// Geomoteric distribution
double CGDoWhileScheduler :: costInGeometric(int num, int resWork) {

	// calculate x
	double procRatio = double(num) / double(numProcs);
	DistGeometric* geo = (DistGeometric*) dist.myParam();
	double p = geo->getP();
	double z = 1 - p;
	double ratio = log(procRatio) / log(p);
	bestX = adjustX(geo->getMin(), int(ratio), resWork, num);
	int dyX = bestX - geo->getMin();
	
	// calculate total cost due to the non-deterministic part.
	double total = tau * 
		(num*bestX + numProcs*pow(p, dyX + 1)/z);

	return total;
}

// Uniform distribution
double CGDoWhileScheduler :: costInUniform(int num, int resWork) {

	// calculate x
	double procRatio = double(num) / double(numProcs);
	DistUniform* uni = (DistUniform*) dist.myParam();
	int tmax = uni->getMax();
	int tmin = uni->getMin();
	double p = tmax - tmin + 1;
	double tsum = procRatio*p;
	double temp = ceil(tsum - 1);
	int tx = tmax - int(temp) - tmin;
	bestX = adjustX(tmin, tx, resWork, num);
	
	// calculate total cost
	int l = tmax - bestX;
	tsum = l * (l+1) / 2;
	double total = tau * (num*bestX + numProcs*tsum/p);

	return total;
}

// adjust "x" - if too much idle time, reduce "x".
int CGDoWhileScheduler :: adjustX(int min, int x, int resWork, int num) {

	if (numProcs == num) return (x + min);

	double make = double (resWork) / double (numProcs - num);
	int newX = min + x;
	// compare with remaining work, the processor-time space in the
	// non-iteration processors.
	if ( (tau*newX) > int(make)) {
		// adjust "x" - reduce it.
		newX = int(make / tau);
		if (tau*newX < make) newX++;
	}
	if (newX < min) newX = min;
	return newX;
}
		

// General distribution
double CGDoWhileScheduler :: costInGeneral(int num, int resWork) {

	// calculate x
	double procRatio = double(num) / double(numProcs);
	DistGeneral* geo = (DistGeneral*) dist.myParam();
	double psum = 0;
	int i = 0;
	// scan the table to get the first guess of "x"
	while (psum < procRatio) {
		psum += geo->getTable(i).value;
		i++;
	}
	int j = i - 1;
	int tempx = geo->getTable(i).index;

	// adjust "x" if too much idle time.
	if (numProcs > num) {
		double make = double(resWork) / double(numProcs - num);
		// compare with remaining work, the processor-time space in 
		// the non-iteration processors.
		if ( (tau*tempx) > int(make)) {
			// adjust "x" - reduce it.
			tempx = (int(make / tau) + 1);
			j = 0;
			while (geo->getTable(j).index > tempx)
				j++;
			j--;
		}
	}
	bestX = tempx;

	// calculate total cost due to the non-deterministic part.
	double tsum = 0;
	for (i = 0; i <= j; i++) {
		psum = double(geo->getTable(i).index - bestX);
		tsum += geo->getTable(i).value * int (psum);
	}
	double total = tau * (num * bestX + tsum * numProcs);

	return total;
}

        ///////////////////////
        //  download the code
        ///////////////////////

int CGDoWhileScheduler :: downLoadCode(CGStar* macroS,
					int invoc, int pix, int pId) {

	CGTarget* t = (CGTarget*) mtarget->child(pId);

	// set up communication star for synchronization
	CGStar* s = 0;
	Geodesic* gd = 0;

	if (pix) {
		mtarget->ddf()->startCode_DoWhile(0, 0, t);
		if (haltRequested()) return FALSE;
	
        	// inside clusterBag.
        	downLoadBodyCode(cbag, pix, t);
		if (haltRequested()) return FALSE;

		// receiving star
		s = syncCommStar(syncReceiveStars, invoc, pix, 1);
		if (!t->incrementalAdd(s)) return FALSE;
	} else {
		// switch geodesic of the clusterBag boundary 
		gd = findGeo(macroS, "Case");
		CGPortHole* tempP1 = boundaryPort(cbag, 0);
		Geodesic* saveG1 = tempP1->geo();
		tempP1->switchGeo(gd);
		CGPortHole* tempP2 = boundaryPort(cbag, 1);
		Geodesic* saveG2 = tempP2->geo();
		tempP2->switchGeo(gd);
		Geodesic* gd1 = findGeo(macroS, "EndCase");
		mtarget->ddf()->startCode_DoWhile(gd, gd1, t);
		if (haltRequested()) return FALSE;
		
        	// inside clusterBag.
        	downLoadBodyCode(cbag, pix, t);
		if (haltRequested()) return FALSE;

		// find Geodesic of the inside clusterBag connection to the
		// control port.
		CGPortHole* tempP = boundaryPort(cbag, 2);
		gd = tempP->geo();
		if (!addSendCodes(gd, invoc, t)) return FALSE;

		tempP1->revertGeo(saveG1);
		tempP2->revertGeo(saveG2);
	}

	mtarget->ddf()->endCode_DoWhile(s, gd, t);
	if (haltRequested()) return FALSE;
	return TRUE;
}

// virtual methods --- default

int CGDoWhileScheduler :: mySyncTime()  { return 2; }

