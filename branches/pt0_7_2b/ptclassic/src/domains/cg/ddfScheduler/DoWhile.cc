static const char file_id[] = "DoWhile.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "DoWhile.h"
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

 Methods for "DoWhile" construct.

**************************************************************************/

	////////////////////////////
	// clearSchedules
	////////////////////////////

void DoWhile :: clearSchedules () {
	schedules[0].initialize();	// EndCase stars and macro
	schedules[1].initialize();	// Case Stars.
	testPort = 0;
}

	////////////////////////////
	// checkTopology
	////////////////////////////

int DoWhile :: checkTopology (CGMacroClusterGal* galaxy) {

	CGMacroClusterGalIter nextStar(*galaxy);
	CGMacroCluster* s;

	// check the topology
	// 1. check whether stars are an clusterBags + non-SDF stars:
	//    Select Stars, or Switch Stars.
	// 2. check the clusterBags are connected to Select, Switch,
	//    or Fork stars.
	// 3. check the control arc and the feedback arcs.
	
	nextStar.reset();
	int mCount = 0;
	while ((s = nextStar++) != 0) {

		if (s->asSpecialBag()) {

			mCount++;
			if (mCount > 1) return FALSE;

			// check connections of the macro.
			CGClustPort* p;
			CGClustPortIter nextp(*s);
	
			while ((p = nextp++) != 0) {

			   // check numberTokens
			   DFPortHole& realP = p->inPtr()->real();
			   if (realP.numXfer() > 1) return FALSE;

			   CGStar* ds = (CGStar*) realP.far()->parent();
			   if (realP.isItInput()) {
				if(!compareType(*ds,"EndCase")) {
				   return FALSE;
				}
			   } else {
			      if ((!compareType(*ds,"Case")) && 
				   (!ds->isItFork()) &&
				  strcmp(realP.far()->name(), "control")) {
				   return FALSE;
			      }
			   }
			}
			schedules[0].put(s);

		} else {
			CGAtomCluster* as = (CGAtomCluster*) s;
			CGStar& ds = as->real();

			// check the connection of the Select Star
			if (compareType(ds, "EndCase")) {

			   PortHole* p;
			   BlockPortIter nextp(ds);
	
			   while ((p = nextp++) != 0) {
				if (p->isItInput()) {
				    const char* tc = p->name();
				    if (!strcmp(tc, "control")) {
					CGStar* ts = 
						(CGStar*) p->far()->parent();
					if (!ts->isItFork())
						return FALSE;
					if ((p->numTokens()) != 1)
						return FALSE;
				   } else if (strcmp(tc, "input#2")) {
					if (!p->aliasFrom()) return FALSE;
				   } else {
					tc = p->far()->name();
					if (strcmp(tc, "output#2"))
						return FALSE;
				   }
				} 
			   } // end of while
			   schedules[0].prepend(s);
			}

			// check the connection of the Switch Star
			else if (compareType(ds, "Case")) {
			   PortHole* p;
			   BlockPortIter nextp(ds);
	
			   while ((p = nextp++) != 0) {
				const char* tc = p->name();
				if (!strcmp(tc, "control")) {
				   testPort = p;
				   CGStar* ts = (CGStar*) p->far()->parent();
				   if (!ts->isItFork())
						return FALSE;
				} else if (!strcmp(tc, "output#1")) {
					if (!p->aliasFrom())
						return FALSE;
				}
			   }
			   schedules[1].put(s);
                        } else if (ds.isItFork()) {
                                PortHole* p;
                                BlockPortIter nextp(ds);
                                while ((p = nextp++) != 0) {
                                        if (p->isItOutput()) {
                                           if(strcmp(p->far()->name(),
                                                "control"))
                                                return FALSE;
					} 
                                }
                                schedules[1].prepend(s);
			} else {
				return FALSE;
			}
		}
	}

	return TRUE;
}

