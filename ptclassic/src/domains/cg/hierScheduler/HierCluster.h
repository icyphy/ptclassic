#ifndef _HierCluster_h
#define _HierCluster_h 1
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

 Programmer: Jose Luis Pino
 Date of Creation: 11/13/95
	
********************************************************************/

#include "Cluster.h"

class HierCluster : public Cluster {
public:
    HierCluster(Star&);
    HierCluster(Cluster&);
    HierCluster(Galaxy&);

    /*virtual*/ int absorb(Block&);    
    /*virtual*/ int merge(Cluster&);
    /*virtual*/ int flatten();

    // The number of repetitions of the cluster in a periodic
    // schedule.  Initialized to 0 by the constructors.  Set to correct
    // value by clustering.
    int repetitions;

private:
    void adjustRepetitions(Block& /*blockToMerge*/);
    int adjustRepetitions(int /*newRepetitionCount*/);
};

int wellOrderCluster(Galaxy&);

#endif



