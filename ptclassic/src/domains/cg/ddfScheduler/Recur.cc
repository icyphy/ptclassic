static const char file_id[] = "Recur.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "Recur.h"
#include "CGMacroCluster.h"
#include "GalIter.h"
#include "State.h"

/**************************************************************************
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

 Programmer:  Soonhoi Ha
 Date of creation: 7/8/91

 methods for "Recur" construct.

**************************************************************************/

CGMacroCluster* lastCluster;
CGMacroCluster* firstCluster;

	////////////////////////////
	// clearSchedules
	////////////////////////////

void Recur :: clearSchedules() {

	selfArm = 0;
	for (int i = 0; i < 4; i++)
		schedules[i].initialize();
}

	////////////////////////////
	// checkTopology
	////////////////////////////

int numSelf;

int Recur :: checkTopology (CGMacroClusterGal* galaxy) {

	CGMacroClusterGalIter nextStar(*galaxy);
	CGMacroCluster* s;

	// check the topology
	// 1. check whether other stars are clusterBags + non-SDF stars:
	//    Self Stars, Select Stars, or Switch Stars.
	// 2. check the connections.
	
	nextStar.reset();
	numSelf = 0;
	while ((s = nextStar++) != 0) {

		if (s->asSpecialBag()) {
			if (checkCluster(s) == FALSE)
				return FALSE;
		} else {
			if (checkStars(s, galaxy->realGal()) == FALSE)
				return FALSE;
		}
	}
	if (!numSelf) return FALSE;

	schedules[2].append(lastCluster);
	schedules[0].prepend(firstCluster);

	return TRUE;
}

int Recur :: checkStars(CGMacroCluster* s, Galaxy* /*galaxy*/) {

	CGAtomCluster* as = (CGAtomCluster*) s;
	CGStar& ds = as->real();

	// check the connection of the EndCase Star
	if (compareType(ds, "EndCase")) {
	   	PortHole* p;
	   	BlockPortIter nextp(ds);
	
		while ((p = nextp++) != 0) {
			if (p->isItInput()) {
			    const char* tc = p->name();
			    if (!strcmp(tc, "control")) {
			   	CGStar* ts = (CGStar*) p->far()->parent();
			   	if (!compareType(*ts, "Fork"))
			      		return FALSE;
			    } 
			} else if (!p->aliasFrom())
				return FALSE;
		} // end of while
		schedules[3].put(s);

	// check the connection of the Case Star
	} else if (compareType(ds, "Case")) {
		PortHole* p;
		BlockPortIter nextp(ds);
	
		while ((p = nextp++) != 0) {
			const char* tc = p->name();
			if (!strcmp(tc, "control")) {
			   CGStar* ts = (CGStar*) p->far()->parent();
			   if (!compareType(*ts, "Fork"))
			      return FALSE;
			}
		}
		schedules[0].put(s);

        } else if (compareType(ds, "Fork")) {
                PortHole* p;
                BlockPortIter nextp(ds);
                while ((p = nextp++) != 0) {
                        if (p->isItOutput()) {
                               if(strcmp(p->far()->name(), "control"))
                                        return FALSE;
			} 
                }
                schedules[0].prepend(s);

	} else if (!compareType(ds, "Self")) {
	    	return FALSE;
	} else {
		numSelf++;
		// State* st = s->stateWithName("recurGal");
		// if (!galaxy) return FALSE;
		// if (st && strcmp(st->initValue(), galaxy->name()))
		// return FALSE;

	   	CGClustPort* p;
	   	CGClustPortIter nextp(*s);
		int errorFlag = 0;
	
		while ((p = nextp++) != 0) {
			CGMacroCluster* fars = p->far()->parentClust();
			if (!fars->asSpecialBag()) {
				CGAtomCluster* faras = (CGAtomCluster*) fars;
				if (p->isItInput()) {
				    if (!compareType(faras->real(),"Case"))
					return FALSE;
				} else {
				    if (!compareType(faras->real(),"EndCase"))
					return FALSE;
				}
				const char* nm = p->far()->real().name();
				int temp = 0;
				if (strcmp(nm, "control"))
					temp = nm[7] - '0';
				else return FALSE;
				if (selfArm && selfArm != temp)
					errorFlag = 1;
			}
		}
		if (errorFlag) {
			Error::abortRun(*s,
"should lie in either true or false arms.");
			return FALSE;
		}
		schedules[2].put(s);
	}

	return TRUE;
}


int Recur :: checkCluster(CGMacroCluster* s) {

	// check connections of the Clusters.
	CGClustPort* p;
	CGClustPortIter nextp(*s);
	int inNum = 0;
	int type = 0;
	int errorFlag = 0;
	int selfFlag = 0;
	
	while ((p = nextp++) != 0) {

		// check numberTokens
		DFPortHole& realP = p->inPtr()->real();
		if (realP.numXfer() > 1) return FALSE;

	    	if (realP.isItInput()) {
	   		if (p->far()) {
				inNum++;
				CGStar* z = (CGStar*) realP.far()->parent();
				if (compareType(*z, "Case")) {
					const char* nm = realP.far()->name();
					if (nm[7] == '2') {
						if (type == 1) errorFlag = 1;
						else type = 2;
					} else {
						if (type == 2) errorFlag = 1;
						else type = 1;
					}
				} else if (compareType(*z, "Self"))
					selfFlag = 2;
				else	return FALSE;
			}

	    	} else {

			CGStar* z = (CGStar*) realP.far()->parent();
			if (compareType(*z, "EndCase")) {
				const char* nm = p->far()->name();
				if (nm[6] == '2') {
					if (type == 1) errorFlag = 1;	
					else type = 2;
				} else if (nm[6] == '1') {
					if (type == 2) errorFlag = 1;
					else type = 1;
				} 
			} else if (compareType(*z, "Self"))
				selfFlag = 1;
			else if (compareType(*z, "Case")) {
				const char* nm = realP.far()->name();
				if (!strcmp(nm, "control")) {
					// initialize testPort.
					testPort = p->far();
				}
			} else if (!compareType(*z, "Fork"))
				return FALSE;
	    	}
	}
		
	// check selfFlag to set the selfArm
	if (selfFlag) {
		if ((selfArm > 0) && (selfArm != type))
			errorFlag = 1;
		else
			selfArm = type;
	}

	// looking at the error flag first.
	if (errorFlag) {
		Error::abortRun(*s, "should lie in either true or false arm.");
		return FALSE;
	}
	
	// next, number of input portholes.
	if (inNum == 0 && type == 0)  {
		firstCluster = s;   	// put at the head.
	  	return TRUE;
	}

	// next, type is undefined.
	if (!type)	return FALSE;

	// next, decide recurArm..
	if (selfFlag == 1) 	schedules[2].prepend(s);
	else if (selfFlag == 2)	lastCluster = s;
	else  		schedules[1].append(s);
					
	return TRUE;
}
