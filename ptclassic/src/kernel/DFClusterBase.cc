static const char file_id[] = "DFClusterBase.cc";
/******************************************************************
Version identification:
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
 Date of creation: 6/10/94

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "DFClusterBase.h"
#include "DataFlowStar.h"
#include "DFPortHole.h"

DFClusterBase::DFClusterBase(Star& self,const char* domain):
Cluster(self,domain) {};

int DFClusterBase::isClusterSDFinContext() const {
    if (isClusterAtomic())
	return Cluster::master?((DataFlowStar*)Cluster::master)->isSDFinContext():TRUE;
    CDFClusterStarIter nextStar(*this);
    const DataFlowStar* s;
    while ((s = nextStar++) != 0)
	if (!s->isSDFinContext()) return FALSE;
    return TRUE;
}

/*virtual*/ int  DFClusterBase::generateSchedule() {
    if (isClusterAtomic()) return TRUE;
    if (!Cluster::generateSchedule()) return FALSE;
    // Now we must adjust all external Cluster porthole parameters
    ClusterPortIter clusterPorts(*this);
    ClusterPort* port;
    while((port = clusterPorts++) != NULL) {
	DFPortHole* internalPort = (DFPortHole*) port->clusterAlias();
	int reps = internalPort->parentReps();
	DFPortHole& dfPort = (DFPortHole&)port->asPort();
	dfPort.setSDFParams(internalPort->numXfer()*reps,
			    internalPort->maxDelay()*reps);
    }
    return TRUE;
}
