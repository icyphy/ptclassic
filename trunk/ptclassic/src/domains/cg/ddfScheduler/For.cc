static const char file_id[] = "For.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "For.h"
#include "GalIter.h"
#include "CGMacroCluster.h"

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

 Methods for "For" construct.

**************************************************************************/

	////////////////////////////
	// clearSchedules
	////////////////////////////

void For :: clearSchedules () {
	schedules[0].initialize();	// UpSample stars and a macro
	schedules[1].initialize();	// main macro
	schedules[2].initialize();	// DownSample stars 
	testPort = 0;
}

	////////////////////////////
	// checkToppology
	////////////////////////////

int For :: checkTopology (CGMacroClusterGal* galaxy) {

	// check the topology
	// 1. check whether stars are all clusterBags + non-SDF stars,
	//    UpSample Stars, or DownSample Stars. 
	// 2. check the clusterBags are connected to an UpSample and 
	//    a DownSample stars.
	
	int numUp = 0;

	CGMacroClusterGalIter nextStar(*galaxy);
	CGMacroCluster* s;

	while ((s = nextStar++) != 0) {

		if (s->asSpecialBag()) {

			// check connections.
			CGClustPort* p;
			CGClustPortIter nextp(*s);
			int flag = 0;
	
			while ((p = nextp++) != 0) {

			   DFPortHole& realP = p->inPtr()->real();
			   if (realP.atBoundary()) {
				flag++;
				continue;
			   }

			   // check the numberTokens
			   if (realP.numXfer() > 1) return FALSE;

			   CGStar* ds = (CGStar*) realP.far()->parent();
			   if (realP.isItInput()) {
				if (!compareType(*ds, "UpSample")) {
				   return FALSE;
				}
			   } else {
			      if ((!compareType(*ds,"DownSample"))  
				&& strcmp(realP.far()->name(), "control"))
				   return FALSE;
			   }
			}
			if (flag == 0) 	   {
				schedules[1].prepend(s);
			}
			else if (flag == 1) schedules[0].prepend(s);
			else		    return FALSE;

		} else {
			CGAtomCluster* as = (CGAtomCluster*) s;
			CGStar& ds = as->real();
			if (compareType(ds, "UpSample")) {
				numUp++;
				schedules[0].put(s);

			} else if (compareType(ds, "DownSample")) {
				PortHole* p;
				BlockPortIter nextp(ds);
				while ((p = nextp++) != 0) {
					if (p->isItInput()) {
					   if (!strcmp(p->name(), "control"))
						testPort = p->far();
					}
				}
				schedules[2].put(s);

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

	if (numUp) noUp = FALSE;
	else	   noUp = TRUE;

	return TRUE;
}

