static const char file_id[] = "Case.cc";
/**************************************************************************
Version identification:
%W5	$Date$

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

 Methods for "Case" construct.

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "Case.h"
#include "GalIter.h"
#include "CGMacroCluster.h"

	////////////////////////////
	// checkNumArcs
	////////////////////////////

void Case :: checkNumArcs(CGMacroClusterGal* galaxy) {

	numArc = 0;

	// check the number of branches
	CGMacroClusterGalIter nextStar(*galaxy);
	CGMacroCluster* s;
	
	while ((s = nextStar++) != 0) {
		if (s->asSpecialBag()) continue;
		CGAtomCluster* as = (CGAtomCluster*) s;
		CGStar& realS = as->real();
		if (compareType(realS, "Case") || 
			compareType(realS, "EndCase")) {
			numArc = realS.numberPorts();
			break;
		}
	}
}

	////////////////////////////
	// clearSchedules
	////////////////////////////

void Case :: clearSchedules () {

	testPort = 0;

	// initialize star list
	if (schedules) { LOG_DEL; delete [] schedules;
			 schedules = NULL; }
	if (numArc) {
		LOG_NEW; schedules = new SequentialList[numArc];
	}
	for (int i = 0; i < numArc; i++)
		schedules[i].initialize();
}

	////////////////////////////
	// checkTopology
	////////////////////////////

int Case :: checkTopology (CGMacroClusterGal* galaxy) {

	// check the number of branches
	CGMacroClusterGalIter nextStar(*galaxy);
	CGMacroCluster* s;
	
	// check the topology
	// 1. check whether stars are all clusterBag + non-SDF stars,
	//    Case Stars, or EndCase Stars.
	// 2. check the clusterBags are connected to  a Case and an EndCase
	//    stars.
	
	nextStar.reset();
	while ((s = nextStar++) != 0) {

		if (s->asSpecialBag()) {

			// check connections.
			CGClustPort* p;
			CGClustPortIter nextp(*s);
			int flag = 0;
			int index = 0;
			int errorFlag = 0;
	
			while ((p = nextp++) != 0) {

			   DFPortHole& realP = p->inPtr()->real();
			   if (realP.atBoundary()) {
				flag++;
				continue;
			   }

			   // check numberTokens
			   if (realP.numXfer() > 1) return FALSE;

			   CGStar* ds = (CGStar*) realP.far()->parent();
			   if (realP.isItInput()) {
				if (!compareType(*ds, "Case")) { 
				   return FALSE;
				} 
				const char* nm = realP.far()->name();
				int temp = nm[7] - '0';	// output#?
				if (index == 0) index = temp;
				else if (index != temp) {
					errorFlag = 1;
				}
			   } else {
			      if (compareType(*ds,"EndCase")) {
				  const char* nm = realP.far()->name();
				  if (strcmp(nm, "control")) {
					int temp = nm[6] - '0';  // input#?
					if (index == 0) index = temp;
					else if (index != temp) 
						errorFlag = 1;
				  }
			      } 
			      else if (strcmp(realP.far()->name(),"control"))
					return FALSE;
			      else if (!compareType(*ds, "Case"))
					return FALSE;
			   }
			}
			if (errorFlag) {
				Error::abortRun(*s,
"may not lie on more than one branch.");
				return FALSE;
			}

			if (flag > 1)		return FALSE;
			else if (flag == 1)  	schedules[0].prepend(s);
			else if (index)	    	schedules[index].append(s);
			else			return FALSE;

		} else {
			CGAtomCluster* as = (CGAtomCluster*) s;
			CGStar& ds = as->real();
			if (compareType(ds, "Case")) {
				PortHole* p;
				BlockPortIter nextp(ds);
	
				while ((p = nextp++) != 0) {
				   if (!strcmp(p->name(),"control"))
					testPort = p;
				}
				schedules[0].put(s);
			} else if (compareType(ds, "EndCase")) {
				PortHole* p;
				BlockPortIter nextp(ds);
	
				while ((p = nextp++) != 0) {
				   if (!strcmp(p->name(),"control"))
					if(!testPort) testPort = p->far();
				}
				int index = numArc - 1;
				schedules[index].put(s);
                        } else if (compareType(ds, "Fork")) {
                                PortHole* p;
                                BlockPortIter nextp(ds);
                                while ((p = nextp++) != 0) {
                                        if (p->isItOutput())
                                           if(strcmp(p->far()->name(),
                                                "control"))
                                                return FALSE;
                                }
                                schedules[0].prepend(s);
			} else {
				return FALSE;
			}
		}
	}

	return TRUE;
}

