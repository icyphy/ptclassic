static const char file_id[] = "CGRecurScheduler.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGRecurScheduler.h"
#include "Galaxy.h"
#include "DataStruct.h"
#include "distributions.h"
#include "KnownBlock.h"
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

 Methods for CGRecurScheduler class.
**************************************************************************/

CGRecurScheduler :: CGRecurScheduler(MultiTarget* t, ostream* l) :
	CGConScheduler(t,l) {
	LOG_NEW; body = (Profile**) new Profile* [4];
	createSchedule();
	dataSendStars.initialize();
	dataReceiveStars.initialize();
	saveK = 0;
	localId = 0;
}

CGRecurScheduler :: ~CGRecurScheduler() {
	LOG_DEL; delete body;
	body = 0;
	dataSendStars.deleteAll();
	dataSendStars.initialize();
	dataReceiveStars.deleteAll();
	dataReceiveStars.initialize();
}

int CGRecurScheduler :: getStatistics() {

	// read the type of the distribution
	if (!dist.setType(refGal,mtarget)) return FALSE;

	// set the paramter information
	if (!dist.myParam()->setParams(refGal, mtarget)) {
		Error :: abortRun("Case: parameter reading error!");
		return FALSE; 
	}
	if (logstrm) {
		*logstrm << "Recur construct with the following statistics:\n";
		*logstrm << dist.myParam()->printParams();
	}
	return TRUE;
}

int wrongTopology() {
	Error::abortRun("wrong Topology for recursion construct");
	return FALSE;
}

int CGRecurScheduler :: examine(CGMacroClusterGal* galaxy) {
        Recur::clearSchedules();
        if (!Recur::checkTopology(galaxy)) {
                Error::abortRun(*galaxy, 
			" has a wrong topology for Recur construct");
                return FALSE;
        }
	return TRUE;
}


int CGRecurScheduler :: closerExamine() {

	int num = 0;
	pathLeng = 0;

	// check up the "schedules" list one by one.
	ListIter top(schedules[0]);
	CGMacroCluster* s;
	while ((s = (CGMacroCluster*) top++) != 0) {
		if (s->asSpecialBag()) {
			num++;
			if (num > 1) {
				return wrongTopology();
			} else {
			   	cbagA = (CGMacroClusterBag*) s;
				body[0] = cbagA->getProfile(0);
				pathLeng += cbagA->myExecTime();
			}
		} else break;
	}
			

	// non-recurion arc: check whether there is only one clusterBag.
	if (schedules[1].size() != 1) {
		return wrongTopology();
	} else {
		cbagB = (CGMacroClusterBag*) schedules[1].head();
		body[1] = cbagB->getProfile(0);
	}

	// recursion arc
	num = 0;
	int temp = 0;
	ListIter recur(schedules[2]);
	while ((s = (CGMacroCluster*) recur++) != 0) {
		if (s->asSpecialBag()) {
			num++;
			if (num > 1) {
				return wrongTopology();
			} else {
				pathLeng += s->myExecTime();
				if (!temp) {
					body[2] = s->getProfile(0);
					cbagC = (CGMacroClusterBag*) s;
				} else {
					body[3] = s->getProfile(0);
					cbagD = (CGMacroClusterBag*) s;
				}
			}
		} else {
			num = 0;
			temp++;
		}
	}

	numSelf = temp;
	return TRUE;
}

	///////////////////////
	//  assumeExecTime
	///////////////////////

int CGRecurScheduler :: assumeExecTime() {

	// get the assumed depth of recursion
	double avg = dist.myParam()->assumedValue();
	double depth;
	if (numSelf <= 1) depth = avg;
	else {
		depth = pow(numSelf, avg) - 1;
		depth = depth / (numSelf - 1);
	}

	return int(depth * double(pathLeng));
}

// local to this file
static int satisfy;		// flag to be set if bestX >= bestK after
				// the normal calculation.

	///////////////////////
	//  setProfile
	///////////////////////

int CGRecurScheduler :: setProfile(int num, int resWork,
			Profile* prof) {

	// compute the optimal number of assigned processors.
	Profile& pa = body[0][num - 1];
	Profile& pb = body[1][num - 1];
	Profile& pc = body[2][num - 1];
	Profile& pd = body[3][num - 1];
	
	// check the effective number of processors
	if (pa.getEffP() < num && pc.getEffP() < num &&
		pd.getEffP() < num) return FALSE;

	// calculate best K value.
	if (numSelf > 1) {
		double temp1 = double(numProcs) / double(num);
		double temp2 = log(temp1) / log(numSelf);
		bestK = int(temp2);
	} else
		bestK = 0;
		
	// calculate tau
	tau = pa.getMakespan() + pc.getMakespan() + pd.getMakespan();
	int preTau = pa.getMakespan() + pc.getMakespan();
	int postTau = pd.getMakespan();
	tauO = pa.getMakespan() + pb.getMakespan();

	// compute the profile
	// If the bestX is smaller than bestK, reduce bestK and repeat.
	double totalCost;
	satisfy = FALSE;
	totalCost = calcCost(num, resWork);
	int tempK = bestK;
	int tempX = bestX;
	while ((!satisfy) && bestK > 0) {
		bestK--;
		double tempCost = calcCost(num, resWork);
		if (tempCost < totalCost) {
			totalCost = tempCost;
			tempK = bestK;
			tempX = bestX;
		}
	} 
	bestK = tempK;
	bestX = tempX;

	// compute the makespan
	int span = 0;
	if (numSelf > 1) {
		float tmp1 = pow(numSelf,bestX-bestK);
		float tmp2 = (tmp1 - 1) / float(numSelf-1);
		span += tau * (int(tmp2) + bestK) + tauO * int(tmp1);
	} else {
		span += tau * bestX + tauO;
	}
	
	// record the profile: shrewed index manipulation.
	int m = int(pow(numSelf, bestK));
	int stop = 0;
	for (int i = 0; i <= bestK; i++) {

		// compute the offset.
		int move = i;
		int start = stop;
		stop = int(pow(numSelf, i));
		for (int j = start; j < stop; j++) {

			// record the profile.
			for (int k = 0; k < num; k++) {
				int index = k + j*num;
				prof->setStartTime(index, move*preTau);
				prof->setFinishTime(index,span - move*postTau);
			}
		}
	}
	
	// add control and synchronization code to the profile
	totalCost += addControlCodes(num, prof);

	prof->setEffP(m*num);
	prof->setTotalCost(totalCost);
	prof->summary();
	return TRUE;
}
	
        ///////////////////////
        //  addControlCodes
        ///////////////////////

int CGRecurScheduler :: addControlCodes(int num, Profile* prof) {

        int ct = myControlTime();
        int st = mySyncTime();
        int temp = (ct+st) * bestK;
	int total = 0;

        int stop = 0;
        for (int i = 0; i <= bestK; i++) {

                // adjust the profile
		// hard to interpret the code.... Sorry.
                int moveFront = i * ct;
		int moveBack  = temp - i * st;
		int added = moveBack - moveFront;
                int start = stop;
                stop = int(pow(numSelf, i));
                for (int j = start; j < stop; j++) {

                        // record the profile.
                        for (int k = 0; k < num; k++) {
                                int index = k + j*num;
				total += added;
                                prof->setStartTime(index,
				  prof->getStartTime(index) + moveFront); 
                                prof->setFinishTime(index,
				  prof->getFinishTime(index) + moveBack);
                        }
                }
        }
	return total;
}

	///////////////////////
	//  calcCost
	///////////////////////

double CGRecurScheduler :: calcCost(int num, int resWork) {

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
double CGRecurScheduler :: costInGeometric(int num, int resWork) {

	// calculate x
	double procRatio = double(num) / double(numProcs);
	DistGeometric* geo = (DistGeometric*) dist.myParam();
	double p = geo->getP();
	int tmin = geo->getMin();

	// value checking
	if (p * numSelf >= 1) {
		Error::abortRun("infinite time required for recursion");
		return 0;
	}

	double z = pow(numSelf,bestK);
	double ratio = log(procRatio * z) / log(p);
	bestX = adjustX(tmin, int(ratio), resWork, num);
	if (bestX < bestK) {
		bestX = bestK;
	} else {
		satisfy = TRUE;
	}
	
	// calculate total cost due to the non-deterministic part.
	double total;
	if (numSelf > 1) {
		double t11 = pow(numSelf, bestX);
		double t12 = (t11 - 1) / (numSelf - 1);
		double t1  = num*(tau*t12 + tauO*t11);
		double t21 = pow(p, bestX - tmin + 1)/(1 - p*numSelf);
		double t22 = pow(numSelf, bestX-bestK);
		double t23 = tau + tauO * (numSelf - 1);
		double t2  = numProcs * t23 * t22 * t21;
		total = t1 + t2;
	} else {
	        total = num * (tau * bestX + tauO) + numProcs*tau*
			pow(p, bestX - tmin + 1) / (1 - p);
	}
	return total;
}

// Uniform distribution
double CGRecurScheduler :: costInUniform(int num, int resWork) {

	// calculate x
	double procRatio = double(num) / double(numProcs);
	DistUniform* uni = (DistUniform*) dist.myParam();
	int tmax = uni->getMax();
	int tmin = uni->getMin();
	double p = tmax - tmin + 1;
	double tsum = procRatio * p * pow(numSelf, bestK);
	int tx = tmax - int(floor(tsum)) - tmin;
	bestX = adjustX(tmin, tx, resWork, num);
	if (bestX < bestK) {
		bestX = bestK;
	} else {
		satisfy = TRUE;
	}
	
	// calculate total cost
	double total;
	if (numSelf > 0) {
		double dk = double (numSelf);
		double t11 = pow(numSelf, bestX);
		double t12 = (t11 - 1) / (dk - 1);
		double t1 = num*(t12 + tauO * t11);
		double t21 = (dk-pow(numSelf,tmax-bestX+1))/(1 - dk)
			    - tmax + bestX;
		double t22 = pow(numSelf, bestX-bestK)/(dk - 1);
		double t23 = tau + tauO * (dk - 1);
		double t2  = numProcs * t23 * t22 * t21 / p;
		total = t1 + t2;
	} else {
		total = num * (tau * bestX + tauO) +
			tau*numProcs*(tmax - bestX)*(tmax - bestX + 1)/2/p;
	}

	return total;
}

// adjust "x" - if too much idle time, reduce "x".
int CGRecurScheduler :: adjustX(int min, int x, int resWork, int num) {

	double pk = pow(numSelf, bestK);	// # parallelism.
	if (numProcs == num * int(pk)) return (x + min);

	double make = double (resWork) / double (numProcs - num * int(pk));

	//makespan
	int tempx = x + min;
	double span;
	if (numSelf > 1) {
		span = (pow(numSelf,tempx-bestK+1)-numSelf)/(numSelf-1)+bestK;
	} else {
		span = tempx;
	}

	// compare with remaining work, the processor-time space in the
	// non-iteration processors.
	int newX = tempx;
	if ( (tau*int(span)) > int(make)) {
		// adjust "x" - reduce it.
		double y = make / double(tau);
		if (numSelf > 1) {
			y = (y - bestK) * (numSelf - 1) + numSelf;
			y = log(y) / log(numSelf) + bestK;
		}
		newX = int(y); 
	}
	if (newX < min) newX = min;
	return newX;
}
		

// General distribution
double CGRecurScheduler :: costInGeneral(int num, int resWork) {

	// calculate x
	double procRatio = double(num) / double(numProcs);
	DistGeneral* geo = (DistGeneral*) dist.myParam();
	double ratio = procRatio * pow(numSelf, bestK);
	double psum = 0;
	int i = 0;
	// scan the table to get the first guess of "x"
	while (psum < ratio) {
		psum += geo->getTable(i).value;
		i++;
	}
	int j = i - 1;
	int tempx = geo->getTable(i).index;

	// adjust "x" if too much idle time.
	double span;
	if (numSelf > 1) {
		span = (pow(numSelf,tempx-bestK+1)-numSelf)/(numSelf-1)+bestK;
	} else {
		span = tempx;
	}

	// compare with remaining work, the processor-time space in the
	double pk = pow(numSelf, bestK);
	if (numProcs > num * int(pk)) {
		double make = double(resWork)/double(numProcs-num*int(pk));
		// compare with remaining work, the processor-time space in 
		// the non-iteration processors.
		if ( (tau*int(span)) > int(make)) {
			// adjust "x" - reduce it.
			double y = make / double(tau);
			if (numSelf > 1) {
				y = (y - bestK) * (numSelf - 1) + numSelf;
				y = log(y) / log(numSelf) + bestK;
			}
			tempx = int(y);
			j = 0;
			while (geo->getTable(j).index > tempx)
				j++;
			j--;
		}
	}
	bestX = tempx;
	if (bestX < bestK) {
		bestX = bestK;
	} else {
		satisfy = TRUE;
	}

	// calculate total cost due to the non-deterministic part.
	double total;
	if (numSelf > 1) {
		double t1 = pow(numSelf,bestX);
		total = num * (tau * (t1 - 1)/(numSelf - 1) + tauO * t1);
	} else {
		total = num * (tau * bestX + tauO);
	}

	double sub = 0;
	for (i = 0; i <= j; i++) {
		if (numSelf > 1) {
			double nu = pow(numSelf,geo->getTable(i).index-bestK)
				- pow(numSelf, bestX-bestK);
			sub += geo->getTable(i).value * nu / (numSelf - 1);
		} else {
			sub += geo->getTable(i).value *
				(geo->getTable(i).index - bestX);
		}
	}
	total += sub*numProcs*(tau + tauO * (numSelf - 1));

	return total;
}

// FIXME: this shouldn't really be static this way.  It must be set
// before any cloning.  Maybe it could be a member of CGRecurScheduler
// (then cloneStar would be a member function)

static const char* domForClone;

inline CGStar* cloneStar(const char* name) {
	return (CGStar*) KnownBlock :: clone(name,domForClone);
}

        ///////////////////////
        //  download the code
        ///////////////////////

int CGRecurScheduler :: downLoadCode(CGStar* macroS,
				int invoc, int pix, int pId) {
	CGTarget* t = (CGTarget*) mtarget->child(pId);

	Geodesic* inG = 0;
	if (pix == 0) inG = findGeo(macroS, 0);	// data input.
	Geodesic* gd = 0;
	if (pix == 0) gd = findGeo(macroS, "EndCase");

	// set the static variable above
	domForClone = macroS->domain();

	dummyStars.initialize();

	if ((numSelf <= 1) || (saveK < 1)) {
		if (!recursionProcedure(invoc, pix, pId, inG, gd, saveK)) {
			dummyStars.deleteAll(); return FALSE;
		}
	}  else {
		// signal to the multitarget that the cluster body
		// should be copied.
		if (optNum > 1) mtarget->ddf()->signalCopy(1);

		int conId = pix / optNum;
		int pgId;
		if (conId == 0) pgId = 0;
		else pgId = int (log(conId) / log(numSelf)) + 1;
		int syncId = pix % optNum;

		CGStar* ds = 0;
		// receiving the control sync. if necessary
		if (pix && (syncId == 0)) {
			// receiving the control sync.
			CGStar* s = syncCommStar(dataReceiveStars,invoc,pix,1);
			if (!s) return FALSE;
			if (!dummyConnect(s, 0)) return FALSE;
			if (!incrementalAdd(t,s)) {
				dummyStars.deleteAll(); return FALSE;
			}

			// if-then-else switch.
			mtarget->ddf()->startCode_Case(s, 0, t);

			// propage control downwards
			PortHole* p = s->portWithName("output");
			addControlSend(p->geo(),invoc,pix,pgId,t,0);

			// do nothing for the first branch
			mtarget->ddf()->middleCode_Case(0,t);

			// receiving the data sync. if necessary.
			ds = syncCommStar(dataReceiveStars, invoc, pix, 3);
			if (!ds) {
				dummyStars.deleteAll(); return FALSE;
			}
			if (!dummyConnect(ds, 0)) {
				dummyStars.deleteAll(); return FALSE;
			}
			if (!incrementalAdd(t, ds)) {
				dummyStars.deleteAll(); return FALSE;
			}
			PortHole* srcP = ds->portWithName("output");
			inG = srcP->geo();

			ds = syncCommStar(dataSendStars, invoc, pix, 4);
			if (!ds) {
				dummyStars.deleteAll(); return FALSE;
			}
			CGStar* con = dummyConnect(ds, 1);
			if (!con) {
				dummyStars.deleteAll(); return FALSE;
			}
			incrementalAdd(t, con);
			gd = ds->portWithName("input")->geo();
		}

		// main body of the code
		int x;
		if (pgId == saveK) {
			x = recursionProcedure(invoc, pix, pId, inG, gd, pgId);
		} else {
			x = recursiveDownLoad(invoc, pix, pId, inG, gd, pgId);
		}
		if (x == 0) {
			dummyStars.deleteAll(); return FALSE;
		}

		// sending the data sync. if necessary.
		if (pix && (syncId == 0)) {
			incrementalAdd(t, ds);
			mtarget->ddf()->endCode_Case(t);
		}
	}
	dummyStars.deleteAll();

	// signal to the multitarget that the copying is completed..
	mtarget->ddf()->signalCopy(0);

	return TRUE;
}

// for saveK >= 1, and numSelf > 1, and pgId < saveK.
// This method will be called recursively.

int CGRecurScheduler :: recursiveDownLoad(int invoc, int pix, 
			int pId, Geodesic* inG, Geodesic* outG, int pgId) {

	CGTarget* t = (CGTarget*) mtarget->child(pId);
	int syncId = pix % optNum;

	// common routine for initial recursion code.
	if (!initRecurCode(invoc, pix, pId, inG, outG, pgId)) return FALSE;

	// send data to other processor group.
	Geodesic* dataG = 0;
	if (syncId == 0) {
		if (!addDataSend(invoc, pix, pgId, t)) return FALSE;
		CGClustPortIter nextp(*cbagC);
		CGClustPort* tempP;
		while ((tempP = nextp++) != 0) {
			if (tempP->isItOutput()) break;
		}
		// dataG = tempP->inPtr()->real().geo();
		dataG = copyPortHole(tempP)->geo();
	}

	int x = 0;
	if (pgId == saveK - 1) {
		x = recursionProcedure(invoc, pix, pId, dataG, outG, pgId+1);
	} else {
		x = recursiveDownLoad(invoc, pix, pId, dataG, outG, pgId+1);
	}
	if ((syncId == 0) && (x == 0)) return FALSE;

	// schedule cbagD
	// receive result from other branches.
	Geodesic* saveGeo = 0;
	CGPortHole* tempP = 0;
	if (syncId == 0) {
		CGStar* ds = cloneStar("BlackHole");
		if (!ds) return FALSE;
		ds->setNameParent("dummy", galaxy());
		dummyStars.put(*ds);
		ds->repetitions = 1;
		if (!addDataReceive(outG, invoc, pix, pgId, t, ds)) 
			return FALSE;
		tempP = boundaryPort(cbagD, 1);
		saveGeo = tempP->geo();
		tempP->switchGeo(outG);
	}

	downLoadBodyCode(cbagD, pix, t);
	if (haltRequested()) return FALSE;
	if (tempP) tempP->revertGeo(saveGeo);

	mtarget->ddf()->endCode_Case(t);

	return TRUE;
}

// Make a recursion procedure.
// saveK = 0, numSelf = 1, or pgId = saveK.

int CGRecurScheduler :: recursionProcedure(int invoc, int pix,
			int pId, Geodesic* inG, Geodesic* outG, int newPG) {
	CGTarget* t = (CGTarget*) mtarget->child(pId);
	int syncId = pix % optNum;
	int conId = pix / optNum;
	int pgId;
	if (conId == 0) pgId = 0;
	else pgId = int (log(conId) / log(numSelf)) + 1;

	// If pgId == newPG and optNum > 1, we do not create the recursion
	// function at the first time: setting the following flag FALSE.
	// In other words, we expand the recursion body into the main code
	// before putting into the recursion code -> necessary trick to
	// manage the communication stars.
	int createFunc = TRUE;
	if ((pgId == newPG) && (optNum > 1)) createFunc = FALSE;

	// procedure declaration
	StringList funcName = "recur_";
	funcName << galaxy()->name();

	CGClustPort* p = 0;
	if (createFunc) {
		// call the function
		if (newPG <= saveK) 
			mtarget->ddf()->
				middleCode_Recur(outG, inG, funcName, t);

		// return type
		if (syncId == 0) {
			CGClustPortIter nextp(*cbagD);
			while ((p = nextp++) != 0) {
				if (p->isItInput()) break;
			}
		}
		mtarget->ddf()->startCode_Recur(inG, p, funcName, t);
	}

	// common code for initial recursion.
	Geodesic* tempG = outG;
	if (createFunc) tempG = 0;

	if (!initRecurCode(invoc, pix, pId, inG, tempG, newPG)) return FALSE;

	Geodesic* dataG = 0;
	if (syncId == 0) {
		if (createFunc) dataG = boundaryPort(cbagB, 1)->geo();
		else dataG = outG;
	}

	// calling recursion function.
	CGStar* ds = 0;
	if (syncId == 0) {
		ds = cloneStar("BlackHole");
		if (!ds) return FALSE;
		dummyStars.put(*ds);
	}

	BlockList selfStarList;
	selfStarList.initialize();

	CGClustPortIter nextCp(*cbagC);
	while ((p = nextCp++) != 0) {
		if (p->isItInput()) continue;
		PortHole* farP = copyPortHole(p);
		CGStar* selfS = 0;
		Geodesic* selfGeo = 0;
		if (syncId == 0) {
			CGMacroCluster* tb = p->far()->parentClust();
			selfS = cloneStar("Self");
			if (!selfS) return FALSE;
			selfS->setNameParent(tb->realName(), galaxy());
			selfStarList.put(*selfS);
			DFPortHole* srcP = 
				(DFPortHole*) selfS->portWithName("output");
			srcP->setSDFParams(1,0);
			PortHole* destP = ds->portWithName("input");
			srcP->connect(*destP, 0);
			selfS->repetitions = 1;
			ds->repetitions = 1;
			incrementalAdd(t, selfS);
			selfGeo = srcP->geo();

			// alias this geodesic to cbagD input.
			CGClustPortIter nextDp(*cbagD);
			CGClustPort* dp;
			while ((dp = nextDp++) != 0) {
				if (dp->far()->parentClust() == tb) break;
			}
			CGPortHole* cp = copyPortHole(dp);
			cp->switchGeo(selfGeo);
		}
		mtarget->ddf()->
			middleCode_Recur(selfGeo,farP->geo(),funcName,t);
	}

	// schedule cbagD
	Geodesic* saveGeo = 0;
	CGPortHole* tempP = 0;
	if (syncId == 0) {
		tempP = boundaryPort(cbagD, 1);
		saveGeo = tempP->geo();
		tempP->switchGeo(dataG);
	}

	downLoadBodyCode(cbagD, pix, t);
	if (haltRequested()) return FALSE;
	if (tempP) tempP->revertGeo(saveGeo);

	selfStarList.deleteAll();
	mtarget->ddf()->endCode_Case(t);

	// closing the construct.
	if (createFunc) {
		mtarget->ddf()->endCode_Recur(dataG, funcName, t);
	} else {
		// recursion function definition.
		if (!recursionProcedure(invoc, pix, pId, inG, 0, newPG + 1)) 
			return FALSE;
	}

	return TRUE;
}

// Common routine for recursionProcedure and recursiveDownLoad

int CGRecurScheduler :: initRecurCode(int invoc, int pix, int pId, 
				Geodesic* inG, Geodesic* outG, int newPG) {
	CGTarget* t = (CGTarget*) mtarget->child(pId);
	int syncId = pix % optNum;
	int conId = pix / optNum;
	int pgId;
	if (conId == 0) pgId = 0;
	else pgId = int (log(conId) / log(numSelf)) + 1;
	
	// schedule cbagA
	CGPortHole* tempP = 0;
	Geodesic* saveGeo = 0;
	if (syncId == 0) {
		tempP = boundaryPort(cbagA, 0);
		saveGeo = tempP->geo();
		tempP->switchGeo(inG);
	}
	downLoadBodyCode(cbagA, pix, t);
	if (haltRequested()) return FALSE;
	if (tempP) tempP->revertGeo(saveGeo);

	// find the control port
	Geodesic* contG = 0;
	CGStar* rs = 0;
	if (syncId == 0) {
		contG = boundaryPort(cbagA,2)->geo();
		int flag = 1;
		if (newPG > pgId) flag = -1;
		if (!addControlSend(contG, invoc, pix, newPG, t, flag)) 
			return FALSE;
	} else {
		// receiving control sync.
		rs = syncCommStar(dataReceiveStars, invoc, pix, 1);
		if (!rs) return FALSE;
		int zz = 0;
		if (pgId == newPG) {
			zz = 1;
			if (!dummyConnect(rs, 0)) return FALSE;
		}
		if (!incrementalAdd(t, rs, zz)) return FALSE;
	}
	mtarget->ddf()->startCode_Case(rs, contG, t);

	// schedule cbagB.
	Geodesic* dataG = 0;
	tempP = 0;
	CGPortHole* tempP2 = 0;
	Geodesic* saveG2 = 0;
	if (syncId == 0) {
		dataG = boundaryPort(cbagA, 1)->geo();
		tempP = boundaryPort(cbagB, 0);
		saveGeo = tempP->geo();
		tempP->switchGeo(dataG);
		if (outG) {
			tempP2 = boundaryPort(cbagB, 1);
			saveG2 = tempP2->geo();
			tempP2->switchGeo(outG);
		}
	}
	downLoadBodyCode(cbagB, pix, t);
	mtarget->ddf()->middleCode_Case(0, t);
	if (haltRequested()) return FALSE;
	if (tempP) tempP->revertGeo(saveGeo);
	if (tempP2) tempP2->revertGeo(saveG2);

	// schedule cbagC
	tempP = 0;
	if (syncId == 0) {
		tempP = boundaryPort(cbagC, 0);
		saveGeo = tempP->geo();
		tempP->switchGeo(dataG);
	}
	downLoadBodyCode(cbagC, pix, t);
	if (haltRequested()) return FALSE;
	if (tempP) tempP->revertGeo(saveGeo);
	return TRUE;
}

        ///////////////////////////////////
        //  Utilities for code generation 
        ///////////////////////////////////

int CGRecurScheduler :: addDataSend(int invoc,int pix, int pgId, CGTarget* t) {
	CGClustPortIter nextCp(*cbagC);
	CGClustPort* p;

	int count = 0;
	while ((p = nextCp++) != 0) {
		if (p->isItInput()) continue;
		if (count <= 0) {
			count++; continue;
		}
		int stride = int(pow(numSelf, pgId)) * optNum;
		int index = pix + stride*count;
		PortHole* farP = copyPortHole(p);
		CGStar* snd = syncCommStar(dataSendStars, invoc, index, 2);
		if (!snd) return FALSE;
		CGPortHole* cp = (CGPortHole*) snd->portWithName("input");
		cp->switchGeo(farP->geo());
		if (!incrementalAdd(t, snd)) return FALSE;
		cp->revertGeo(0);
		count++;
	}
	return TRUE;
}

int CGRecurScheduler :: addDataReceive(Geodesic* gd, int invoc, int pix, 
					int pgId, CGTarget* t, CGStar* ds) {
	CGClustPortIter nextCp(*cbagC);
	CGClustPort* p;

	int count = 0;
	int stride = int(pow(numSelf, pgId)) * optNum;
	while ((p = nextCp++) != 0) {
		if (p->isItInput()) continue;

		CGMacroCluster* temps = p->far()->parentClust();
		CGClustPortIter nextDp(*cbagD);
		CGClustPort* dp;
		while ((dp = nextDp++) != 0) {
			if (dp->far()->parentClust() == temps) break;
		}
		CGPortHole* farP = copyPortHole(dp);
		if (count <= 0) {
			farP->switchGeo(gd);
			count++;
			continue;
		}

		int index = pix + stride*count;
		CGStar* rs = syncCommStar(dataReceiveStars, invoc, index, 5);
		DFPortHole* srcP, *destP;
		srcP = (DFPortHole*) rs->portWithName("output");
		srcP->setSDFParams(1,0);
		destP = (DFPortHole*) ds->portWithName("input");
		srcP->connect(*destP, 0);
		rs->repetitions = 1;
		if (!incrementalAdd(t, rs)) return FALSE;

		// connect to the cbagD.
		farP->switchGeo(srcP->geo());
		count++;
	}
	return TRUE;
}

int CGRecurScheduler :: addControlSend(Geodesic* gd, int invoc, int pix,
					int pgId, CGTarget* t, int flag) {
	for (int i = 1; i < optNum; i++) {
		int temp = pix + i;
		CGStar* snd = syncCommStar(dataSendStars, invoc, temp, 0);
		if (!snd) return FALSE;
		CGPortHole* p = (CGPortHole*) snd->portWithName("input");
		p->switchGeo(gd);
		int zz = 0;
		if (flag > 0) zz = 1;
		if (!incrementalAdd(t, snd, zz)) return FALSE;
		p->revertGeo(0);
	}
	if (pgId >= saveK) return TRUE;

	int stride = int(pow(numSelf, pgId)) * optNum;
	for (i = 1; i < numSelf; i++) {
		int temp = pix + stride*i;
		CGStar* snd = syncCommStar(dataSendStars, invoc, temp, 0);
		if (!snd) return FALSE;
		CGPortHole* p = (CGPortHole*) snd->portWithName("input");
		p->switchGeo(gd);
		if (!incrementalAdd(t, snd, flag)) return FALSE;
		p->revertGeo(0);
	}
	return TRUE;
}

CGStar* CGRecurScheduler :: dummyConnect(CGStar* s, int direction) {
	CGStar* ds;
	if (!direction) {
		ds = cloneStar("BlackHole");
	} else {
		ds = cloneStar("Const");
	}
	if (!ds) return FALSE;

	StringList temp = "dummy";
	temp << localId++;
	ds->setNameParent(hashstring(temp), 0);

	DFPortHole* srcP, *destP;
	if (!direction) {
		srcP = (DFPortHole*) s->portWithName("output");
		srcP->setSDFParams(1,0);
		destP = (DFPortHole*) ds->portWithName("input");
	} else {
		srcP = (DFPortHole*) ds->portWithName("output");
		destP = (DFPortHole*) s->portWithName("input");
	}
	srcP->connect(*destP, 0);
	s->repetitions = 1;
	ds->repetitions = 1;

	dummyStars.put(*ds);
	return ds;
}

int CGRecurScheduler :: incrementalAdd(CGTarget* t, CGStar* s, int flag) {
	mtarget->ddf()->signalCopy(0);
	int zz = t->incrementalAdd(s, flag);
	if ((saveK >= 1) && (numSelf > 1) && (optNum > 1)) 
		mtarget->ddf()->signalCopy(1);
	return zz;
}

        ///////////////////////////////////
        //  buildDataSyncs
        ///////////////////////////////////

void CGRecurScheduler :: buildDataSyncs(Profile* pf) {
	if ((saveK < 1) || (numSelf <= 1)) return;

	// reset star lists
	dataSendStars.deleteAll();
	dataSendStars.initialize();
	dataReceiveStars.deleteAll();
	dataReceiveStars.initialize();

	int numSyn = int(pow(numSelf, saveK));

	for (int i = 1; i <= pf->numInstance(); i++) {
		// control syncs first
		for (int k = 0; k < numSyn; k++) {
			int refP = k * optNum;
			int syncP = pf->assignedTo(i, refP);
			Target* t = mtarget->child(syncP);
			int pgId;
			if (!refP) pgId = 0;
			else pgId = int (log(refP) / log(numSelf)) + 1;

			for (int m = pgId; m < saveK; m++) {
			   int stride = int(pow(numSelf, m)) * optNum;
			   for (int n = 1; n < numSelf; n++) {
				int temp = refP + stride * n;
				int to = pf->assignedTo(i, temp);
				Target* dt = mtarget->child(to);
				makeLink(syncP,to,t,dt,i,temp,0);
			   }
			}
			for (m = 1; m < optNum; m++) {
				int temp = refP + m;
				int to = pf->assignedTo(i, temp);
				Target* dt = mtarget->child(to);
				makeLink(syncP,to,t,dt,i,temp,0);
			}
		}
		
		// data sync
		for (k = 0; k < numSyn; k++) {
			int refP = k*optNum;
			int syncP = pf->assignedTo(i, refP);
			Target* t = mtarget->child(syncP);
			int pgId;
			if (!refP) pgId = 0;
			else pgId = int (log(refP)/log(numSelf)) + 1;
			if (pgId == saveK) break;

			for (int m = pgId; m < saveK; m++) {
				int stride = int(pow(numSelf, m)) * optNum;
				for (int n = 1; n < numSelf; n++) {
				   int temp = refP + stride*n;
				   int to = pf->assignedTo(i, temp);
				   Target* dt = mtarget->child(to);
				   makeLink(syncP,to,t,dt,i,temp,2);
				   makeLink(to,syncP,dt,t,i,temp,4);
				}
			}
		}
	}
}
		
void CGRecurScheduler :: makeLink(int from, int to,
		Target* srcT, Target* desT, int invoc, int pix, int flag) {
	DataFlowStar* newS, *newR;
	newS = mtarget->createSend(from, to, 1);
	newR = mtarget->createReceive(from, to, 1);
	newS->setTarget(srcT);
	newR->setTarget(desT);
	newS->setNameParent(nameComm(invoc, pix, flag), 0);
	newR->setNameParent(nameComm(invoc, pix, flag + 1), 0);
	dataSendStars.put(*newS);
	dataReceiveStars.put(*newR);
	mtarget->pairSendReceive(newS, newR);
}

        ///////////////////////////////////
        //  buildDataSyncs
        ///////////////////////////////////

void CGRecurScheduler :: buildSyncLinks(Profile* pf) {
	if ((numSelf <= 1) || (saveK < 1) || (optNum <= 1)) return;

	int numBody = int(pow(numSelf, saveK));
	mtarget->ddf()->prepCode(mtarget, pf, optNum, numBody);
} 

// virtual methods --- default

int CGRecurScheduler :: myControlTime() { return 2;}
int CGRecurScheduler :: mySyncTime() { return 2;}


