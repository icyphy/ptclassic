#ifndef _HierCluster_h
#define _HierCluster_h 1
#ifdef __GNUG__
#pragma interface
#endif


/*******************************************************************
 SCCS Version identification :
 @(#)HierCluster.h	1.4	11/28/95

Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer: Jose Luis Pino
 Date of Creation: 11/13/95
	
********************************************************************/

#include "Cluster.h"

class HierCluster : public Cluster {
public:

    HierCluster():Cluster(), repetitions(0), execTime(0) {};

    /*virtual*/ Block* makeNew() const { return new HierCluster; }
    
    /*virtual*/ int absorb(Cluster&, int=TRUE);    
    /*virtual*/ int merge(Cluster&, int=TRUE);
    /*virtual*/ int flatten(Galaxy* = NULL, int = TRUE);

    /*virtual*/ int flattenTest(Galaxy&);

    /*virtual*/ Cluster* convertGalaxy(Galaxy&);
    /*virtual*/ Cluster* convertStar(Star&);

    // The number of repetitions of the cluster in a periodic
    // schedule.  Initialized to 0 by the constructors.  Set to correct
    // value by clustering.
    int repetitions;

    int execTime;
private:
    void adjustRepetitions(HierCluster& /*clusterToMerge*/);
    int adjustRepetitions(int /*newRepetitionCount*/);
};

#endif



