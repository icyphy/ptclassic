static const char file_id[] = "CGForScheduler.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGForScheduler.h"
#include "Galaxy.h"
#include "DataStruct.h"
#include "distributions.h"
#include "KnownBlock.h"
#include "Geodesic.h"
#include "CGDDFCluster.h"
#include "dataType.h"
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

 Methods for CGForScheduler class.
**************************************************************************/

CGForScheduler :: CGForScheduler(MultiTarget* t, ostream* l) : 
	CGConScheduler(t,l), body(0), cbag(0), saveK(1) {
	For :: createSchedule();
	dataSendStars.initialize();
	dataReceiveStars.initialize();
	localId = 0;
}

CGForScheduler :: ~CGForScheduler() {
	dataSendStars.deleteAll();
	dataSendStars.initialize();
	dataReceiveStars.deleteAll();
	dataReceiveStars.initialize();
}

int CGForScheduler :: getStatistics() {

	// read the type of the distribution
	if (!dist.setType(refGal,mtarget)) return FALSE;

	// set the paramter information
	if (!dist.myParam()->setParams(refGal, mtarget)) {
		Error :: abortRun("Case: parameter reading error!");
		return FALSE; 
	}
	if (logstrm) {
		*logstrm << "For construct with the following statistics:\n";
		*logstrm << dist.myParam()->printParams();
	}
	return TRUE;
}

int CGForScheduler :: examine(CGMacroClusterGal* galaxy) {
        For::clearSchedules();
        if (!For::checkTopology(galaxy)) {
           Error::abortRun(*galaxy, " has wrong topology for For construct ");
           return FALSE;
        }
	return TRUE;
}

int CGForScheduler :: closerExamine() {

	// check whether there is only one clusterBag or more.
	if (schedules[1].size() != 1) {
		Error::abortRun("For construct wants a single clusterBag!");
		return FALSE;
	}
	return TRUE;
}

	///////////////////////
	//  initMembers
	///////////////////////

void CGForScheduler :: initMembers() {
	// Obtain the profile of the iteration body.
	ListIter bodyMacro(schedules[1]);
	cbag = (CGMacroClusterBag*) bodyMacro++;
	body = cbag->getProfile(0);
}

	///////////////////////
	//  assumeExecTime
	///////////////////////

int CGForScheduler :: assumeExecTime() {

	// take the total work of the clusterBag
	int work = cbag->myExecTime();
	double avg = dist.myParam()->assumedValue();
	avg = avg * double(work);

	return int(avg);
}

	///////////////////////
	//  setProfile
	///////////////////////

int CGForScheduler :: setProfile(int num, int resWork,
			Profile* prof) {

	// compute the optimal number of assigned processors.
	Profile& p = body[num-1];
	
	// check the effective number of processors
	if (p.getEffP() < num) return FALSE;

	// calculate best K value.
	if (owner->isParallelizable()) {
		bestK = numProcs / num;
		int tk = p.ToverK();
		if (tk && bestK > tk) bestK = tk;
	} else {
		bestK = 1;
	}

	// compute the profile
	tau = p.getMakespan();
	double totalCost = calcCost(num, resWork);

	// record the profile
	int offset = p.getMinDisplacement();
	int m1, m2, span;
	m1 = bestX / bestK;
	m2 = bestX % bestK;
	for (int i = 0; i < bestK; i++) {
		int move = offset * i;
		if (i < m2) span = (m1 + 1) * tau; 
		else	    span = m1 * tau;
		for (int j = 0; j < num; j++) {
			int index = j + i * num;
			prof->setStartTime(index, move);
			prof->setFinishTime(index, move + span);
		}
	}
	
	// add control codes to the profile.
	totalCost += addControlCodes(num, prof);

	prof->setEffP(bestK*num);
	prof->setTotalCost(totalCost);
	prof->summary();
	return TRUE;
}
	
        ///////////////////////
        //  addControlCodes
        ///////////////////////

int CGForScheduler :: addControlCodes(int num, Profile* prof) {

	ListIter up(schedules[0]);
	ListIter down(schedules[2]);
	CGStar* s;

	// add upSample codes
        int sumUp = 0;
	
	while ((s = (CGStar*) up++) != 0) {
		sumUp += s->myExecTime();
	}
        prof->setFinishTime(0, prof->getFinishTime(0) + sumUp);
	int temp = num * bestK;
        for (int i = 1; i < temp; i++) {
                prof->setStartTime(i, prof->getStartTime(i) + sumUp);
                prof->setFinishTime(i, prof->getFinishTime(i) + sumUp);
        }

	// add downSample codes
        int sumDown = 0;
	while ((s = (CGStar*) down++) != 0) {
		sumDown += s->myExecTime();
	}

	int ix = num * (bestX % bestK - 1);
	if (ix < 0) ix = num * (bestK - 1);
        prof->setFinishTime(ix, prof->getFinishTime(ix) + sumDown);

        return (sumUp + sumDown);
}

	///////////////////////
	//  calcCost
	///////////////////////

double CGForScheduler :: calcCost(int num, int resWork) {

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
double CGForScheduler :: costInGeometric(int num, int resWork) {

	// calculate x
	double procRatio = double(num) / double(numProcs);
	DistGeometric* geo = (DistGeometric*) dist.myParam();
	double p = geo->getP();
	double z = 1 - pow(p,bestK);
	double temp = procRatio * z / (1 - p);
	double ratio = log(temp) / log(p);
	bestX = adjustX(geo->getMin(), int(ratio), resWork, num);
	int dyX = bestX - geo->getMin();
	
	// calculate total cost due to the non-deterministic part.
	double total = tau * 
		(num*bestX + numProcs*pow(p, dyX + 1)/z);

	return total;
}

// Uniform distribution
double CGForScheduler :: costInUniform(int num, int resWork) {

	// calculate x
	double procRatio = double(num) / double(numProcs);
	DistUniform* uni = (DistUniform*) dist.myParam();
	int tmax = uni->getMax();
	int tmin = uni->getMin();
	double p = tmax - tmin + 1;
	double tsum = procRatio*p;
	double temp = ceil(tsum - 1) * bestK;
	int tx = tmax - int(temp) - tmin;
	bestX = adjustX(tmin, tx, resWork, num);
	
	// calculate total cost
	double l = floor( double(tmax - bestX)/double(bestK) );
	// keep the second term since bestX can be modified by adjustX().
	tsum = bestK * l * (l+1) / 2 + (l+1) * (tmax - bestX - l * bestK);
	double total = tau * (num*bestX + numProcs*tsum/p);

	return total;
}

// adjust "x" - if too much idle time, reduce "x".
// But retain the skewed structure.
int CGForScheduler :: adjustX(int min, int x, int resWork, int num) {

	int newX = min + x;
	if (numProcs == num * bestK) return newX;

	double make = double (resWork) / double (numProcs - num * bestK);
	int temp = newX / bestK;
	int skew = newX % bestK;
	// compare with remaining work, the processor-time space in the
	// non-iteration processors.
	if ( (tau*temp) > int(make)) {
		// adjust "x" - reduce it.
		temp = int(make / tau);
		if (temp*tau == make && temp > 0) temp--;
		if (skew == 0) skew = bestK;
		newX = temp*bestK + skew;
	}
	if (newX < min) newX = (min/bestK)*bestK + skew;
	return newX;
}
		

// General distribution
double CGForScheduler :: costInGeneral(int num, int resWork) {

	// calculate x
	double procRatio = double(num) / double(numProcs);
	DistGeneral* geo = (DistGeneral*) dist.myParam();
	double psum = 0;
	int size = geo->tableSize();

	// Among "bestK" values of x, obtain the largest
	// Then, the "bestX" is among [x, x+bestK].
	double limit = procRatio*double(bestK);
	int i = 0;
	while (psum < limit && i < size) {
		psum += geo->getTable(i).value;
		i++;
	}

	int largestX = geo->getTable(i-1).index - 1 + bestK;

	// adjust "largestX" if too much idle time.
	if (numProcs > num * bestK) {
		double make = double(resWork) / double(numProcs - num * bestK);
		int temp = largestX / bestK;
		// compare with remaining work, the processor-time space in 
		// the non-iteration processors.
		if ( (tau*temp) > int(make)) {
			// adjust "largestX" - reduce it.
			largestX = (int(make / tau) + 1) * bestK;
		}
	}

	// Compare total costs with x with [largestX-bestK+1, largestX].
	// Select x with smallest total cost.
	double minSum = numProcs * tau * geo->getTable(0).index;
	bestX = largestX;		// initialize.

	for (int k = 0; k > bestK; k++) {
		int tempx = largestX - k;

		// calculate total cost due to the non-deterministic part.
		double tot = 0;
		i = 0;
		int ix = geo->getTable(i).index;
		while (ix > tempx && i < size) {
			psum = double(ix - tempx)/double(bestK);
			tot += geo->getTable(i).value * ceil(psum);
			i++;
			if (i < size) ix = geo->getTable(i).index;
		}
		tot = (tot*numProcs + num * tempx) *tau;

		// update minSum and optimal X.
		if (tot < minSum) {
			minSum = tot;
			bestX = tempx;
		}
	}

	return minSum;
}

        ///////////////////////
        //  download the code
        ///////////////////////

int CGForScheduler :: downLoadCode(CGStar* macroS,
				   int invoc, int pix, int pId) {

	CGTarget* t = (CGTarget*) mtarget->child(pId);

	// create a domain-specific upsample/downsample star
	CGStar* cs = 0;		// upsample star
	CGStar* d1 = 0;		// dummy blackhole to be connected to upsample.
	CGStar* ds = 0;		// downsample star
	CGStar* d2 = 0;		// dummy Const to be connected to downsample.
	Geodesic* upGeo = 0;
	Geodesic* downGeo = 0;
	const char* inDom = macroS->domain();
	int realPix = pix % optNum;

	if (realPix == 0) {
		// upsample star 
		ListIter next(schedules[0]);
		CGStar* s;
		while((s = (CGStar*) next++) != 0) {
			if (!strcmp(s->readTypeName(), "UpSample")) break;
		}
		if (s) {
			// replace it with a domain-specific star
			cs = (CGStar*) s->clone();
			d1 = (CGStar*)KnownBlock :: clone("BlackHole",inDom);
			if ((!cs) || (!d1)) return FALSE;
			cs->setNameParent(s->name(), s->parent());
			DFPortHole* srcP = (DFPortHole*) 
						cs->portWithName("output");
			srcP->setSDFParams(1, 0);
			PortHole* destP = d1->portWithName("input");
			srcP->connect(*destP, 0);
			cs->repetitions = 1;
			d1->repetitions = 1;
			upGeo = srcP->geo();
		}
		// down sample star
		ListIter next2(schedules[2]);
		while((s = (CGStar*) next2++) != 0) {
			if (!strcmp(s->readTypeName(), "DownSample")) break;
		}
		if ((!pix) && s) {
			// replace it with a domain-specific star
			ds = (CGStar*) s->clone();
			d2 = (CGStar*) KnownBlock :: clone("Const",inDom);
			if ((!cs) || (!d2)) return FALSE;
			ds->setNameParent(s->name(), s->parent());
			StringList temp = "Const";
			temp << "_auto" << localId++;
			d2->setNameParent(hashstring((const char*) temp), 0);
			PortHole* srcP = d2->portWithName("output");
			DFPortHole* destP = (DFPortHole*) 
						ds->portWithName("input");
			destP->setSDFParams(1,0);
			srcP->connect(*destP, 0);
			ds->repetitions = 1;
			ds->repetitions = 1;
			downGeo = srcP->geo();
		}
	}

	// set up communication star for synchronization
	CGStar* commS = 0;
	CGStar* dataS = 0;
	Geodesic* gd = 0;
	Geodesic* dataG = 0;

	if (pix) {
		commS = syncCommStar(syncReceiveStars, invoc, pix, 1);
		if (!incrementalAdd(t, commS)) return FALSE;

		if ((saveK > 1) && (realPix == 0) && upGeo) {
			int temp = pix/optNum;
			dataS = syncCommStar(dataReceiveStars, invoc,temp, 3);
			if (cs) {
				PortHole* srcP = dataS->portWithName("output");
				PortHole* destP = cs->portWithName("input");
				srcP->connect(*destP, 0);
			}
			if (!incrementalAdd(t,dataS)) return FALSE;
		} 
	} else {
		// find control port
		gd = findGeo(macroS, "control");

		// Send stars for control sync.
		if (!addSendCodes(gd, invoc, t)) return FALSE;

		// send stars for data sync. if necessary
		dataG = findGeo(macroS, "UpSample");
		if (saveK > 1) {
			if (dataG) {
				if (!dataSendCodes(dataG, invoc, t)) 
					return FALSE;
			}
		}
		if (d2) incrementalAdd(t, d2);
		if (cs) {
			CGPortHole* p = (CGPortHole*)cs->portWithName("input");
			p->switchGeo(dataG);
			if (p->type() == ANYTYPE) 
				p->inheritTypeFrom(*dataG->sourcePort());
			p = (CGPortHole*)cs->portWithName("control");
			if (p) p->switchGeo(gd);
		}
		if (ds) {
			ds->setTarget(t);
			dataG = findGeo(macroS, "DownSample");
			CGPortHole* p =(CGPortHole*)ds->portWithName("output");
			p->switchGeo(dataG);
			if (p->type() == ANYTYPE) 
				p->inheritTypeFrom(*dataG->destPort());
			p = (CGPortHole*) ds->portWithName("control");
			if (p) p->switchGeo(gd);
		}
	}

	mtarget->ddf()->startCode_For(commS, gd, t);
	if (cs) {
		cs->setTarget(t);
		if (pix) {
			PortHole* destP = cs->portWithName("input");
			CGPortHole* p = (CGPortHole*)
						cs->portWithName("control");
			if (p) p->switchGeo(destP->geo());
		}
		if (!incrementalAdd(t, cs)) return FALSE;

		// revert the connections back.
		CGPortHole* p = (CGPortHole*) cs->portWithName("input");
		if (p->switched()) p->revertGeo(0);
		p = (CGPortHole*) cs->portWithName("control");
		if (p->switched()) p->revertGeo(0);
	}

	int modulo = pix / optNum;
	if (!pix) {
		for (int i = 1; i < saveK; i++) {
		    CGPortHole* outp = boundaryPort(cbag, 1);
		    if (outp) {
		        dataS = syncCommStar(dataReceiveStars, invoc, i, 5);
			CGPortHole* p = (CGPortHole*) 
				dataS->portWithName("output");
			p->switchGeo(downGeo);
			mtarget->ddf()->middleCode_For(dataS, ds, saveK, i, t);
			p->revertGeo(0);
		    } else {
			mtarget->ddf()->middleCode_For(0, 0, saveK, modulo, t);
		    }
		}
	} else {
		mtarget->ddf()->middleCode_For(0, 0, saveK, modulo, t);
	}

        // inside clusterBag.
	if ((saveK > 1) && (optNum > 1)) mtarget->ddf()->signalCopy(1);

	CGPortHole* inp = 0;
	CGPortHole* outp = 0;
	Geodesic* saveInG, *saveOutG;
	if (realPix == 0) {
		inp = boundaryPort(cbag, 0);
		if (inp) {
			saveInG = inp->geo();
			inp->switchGeo(upGeo);
		}
		if (pix == 0) {
			outp = boundaryPort(cbag, 1);
			if (outp) {
				saveOutG = outp->geo();
				outp->switchGeo(downGeo);
			}
		}
	}
        downLoadBodyCode(cbag, pix, t);
        
	// revert back to the original connection
	if (inp) inp->revertGeo(saveInG);
	if (outp) outp->revertGeo(saveOutG);

	if (realPix == 0) {
		outp = boundaryPort(cbag, 1);
		if (pix && outp) {
			// send final result to the sync Proc.
			int temp = pix/optNum;
			dataS = syncCommStar(dataSendStars,invoc,temp,4);
			CGPortHole* p = (CGPortHole*) 
					 dataS->portWithName("input");
			p->switchGeo(outp->geo());
			if (!incrementalAdd(t, dataS)) return FALSE;;
			p->revertGeo(0);
		} else if (outp) {	// pix = 0
			// schedule downSample star.
			int zz = 1;
			if (saveK > 1) zz = 0;
			if (!incrementalAdd(t, ds, zz)) return FALSE;
		}
	}

	mtarget->ddf()->endCode_For(t);

	if (ds) {
		CGPortHole* xp = (CGPortHole*) ds->portWithName("output");
		if (xp->switched()) xp->revertGeo(0);
		xp = (CGPortHole*) ds->portWithName("control");
		if (xp->switched()) xp->revertGeo(0);
	}

	// reclaim the memory
	LOG_DEL; delete cs;
	LOG_DEL; delete ds;
	LOG_DEL; delete d1;
	LOG_DEL; delete d2;

	if ((saveK > 1) && (optNum > 1)) mtarget->ddf()->signalCopy(0);
	return TRUE;
}

int CGForScheduler :: incrementalAdd(CGTarget* t, CGStar* s, int flag) {
        mtarget->ddf()->signalCopy(0);
        int zz = t->incrementalAdd(s, flag);
        if ((saveK > 1) && (optNum > 1)) mtarget->ddf()->signalCopy(1);
        return zz;
}

int CGForScheduler :: dataSendCodes(Geodesic* gd, int invoc, CGTarget* t) {
	CGStar* s;
	for (int i = 1; i < saveK; i++) {
		s = syncCommStar(dataSendStars, invoc, i, 2);
		CGPortHole* p = (CGPortHole*) s->portWithName("input");
		if (!p) {
			Error :: abortRun(*s, "no input porthole");
			return FALSE;
		}
		p->switchGeo(gd);
		if (!incrementalAdd(t, s)) return FALSE;
		p->revertGeo(0);
	}
	return TRUE;
}

        ///////////////////////
        //  buildSyncLinks 
        ///////////////////////

void CGForScheduler :: buildSyncLinks(Profile* pf) {

	if ((optNum > 1) && (saveK > 1)) {
		mtarget->ddf()->prepCode(mtarget, pf, optNum, saveK);
	}
	CGConScheduler :: buildSyncLinks(pf);
}

        ///////////////////////
        //  buildDataSyncs 
        ///////////////////////

void CGForScheduler :: buildDataSyncs(Profile* pf) {
	if (saveK <= 1) return;
	CGPortHole* inp = boundaryPort(cbag, 0);
	CGPortHole* outp = boundaryPort(cbag, 1);
	if ((!inp) && (!outp)) return;

	// reset star lists
	dataSendStars.deleteAll();
	dataSendStars.initialize();
	dataReceiveStars.deleteAll();
	dataReceiveStars.initialize();

	DataFlowStar* newS, *newR;
	for (int i = 1; i <= pf->numInstance(); i++) {
		int syncP = pf->assignedTo(i,0);
		Target* t = mtarget->child(syncP);

		// create Send and Receive for data sync. 
		for (int j = 1; j < saveK; j++) {
			int to = pf->assignedTo(i, j*optNum);

			// at the beginning of For
			if (inp) {
				newS = mtarget->createSend(syncP,to,1);
				newR = mtarget->createReceive(syncP,to,1);
				newS->setTarget(t);
				newR->setTarget(mtarget->child(to));
				newS->setNameParent(nameComm(i,j,2), 0);
				newR->setNameParent(nameComm(i,j,3), 0);
				dataSendStars.put(*newS);
				dataReceiveStars.put(*newR);
				mtarget->pairSendReceive(newS, newR);
			}

			// at the end of For
			if (outp) {
				newS = mtarget->createSend(to,syncP,1);
				newR = mtarget->createReceive(to,syncP,1);
				newS->setTarget(mtarget->child(to));
				newR->setTarget(t);
				newS->setNameParent(nameComm(i,j,4), 0);
				newR->setNameParent(nameComm(i,j,5), 0);
				dataSendStars.put(*newS);
				dataReceiveStars.put(*newR);
				mtarget->pairSendReceive(newS, newR);
			}
		}
	}
}
	
