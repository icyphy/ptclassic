#ifndef _DFClusterBase_h
#define _DFClusterBase_h 1
#ifdef __GNUG__
#pragma interface
#endif

/*******************************************************************
 SCCS Version identification :
 $Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer : Jose Luis Pino
 Date of Creation : 7/11/94
	
********************************************************************/

#include "Cluster.h"
class DataFlowStar;

class DFClusterBase : public Cluster {
public:

    // Constructors
    DFClusterBase(Star& self,const char* domain);

    // All cluster/star classes should redefine isSDFinContext
    // (a DataFlowStar virtual method) which calls this function.
    virtual int isClusterSDFinContext() const;

    /*virtual*/ int generateSchedule();
};

class DFClusterStarIter : private ClusterIter {
public:
    DFClusterStarIter(Cluster&n):ClusterIter(n) {};

    DataFlowStar* next() {
	Cluster* cluster = ClusterIter::next();
	return cluster? (DataFlowStar*) &cluster->star() :(DataFlowStar*)NULL ;
    }
    
    DataFlowStar* operator++(POSTFIX_OP) { return next();}

    ClusterIter::reset;
}; 

class CDFClusterStarIter : private CClusterIter {
public:
    CDFClusterStarIter(const Cluster&n):CClusterIter(n) {};

    const DataFlowStar* next() {
	const Cluster* cluster = CClusterIter::next();
	return cluster ?
	    (const DataFlowStar*)&cluster->star():
	    (const DataFlowStar*)NULL ;
    }
    
    const DataFlowStar* operator++(POSTFIX_OP) { return next();}

    CClusterIter::reset;
}; 
#endif





