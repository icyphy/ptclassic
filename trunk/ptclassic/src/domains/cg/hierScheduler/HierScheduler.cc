static const char file_id[] = "MultiScheduler.cc";

/*****************************************************************
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

Programmer: Jose Luis Pino

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MultiScheduler.h"

int MultiScheduler::dagNodes() const {
    return topScheduler.dagNodes();
}

void MultiScheduler :: setup () {
    if (!galaxy()) {
	Error::abortRun("MultiScheduler: no galaxy!");
	return;
    }
    
    if (!checkConnectivity()) return;

    // Need to initialize galaxy BEFORE constructing the Clusters.
    // If not, the porthole parameters are not properly propigated.
    galaxy()->initialize();
    
    // Need to set repetitions of the master galaxy
    repetitions();

    LOG_NEW; topCluster = new CGCluster(galaxy()->domain());
    
    // We should set the target of the topCluster before construction
    // the clusters for the galaxy, so that all the internal clusters
    // have their target set correctly
    topCluster->setMasterBlock(galaxy());
    topCluster->setTarget(&target());

    // set the top level scheduler
    topScheduler.setTarget(target());
    topCluster->setInnerSched(&topScheduler);

    // set the inner schedulers
    FatClusterIter next(*topCluster);
    CGCluster* fatCluster;
    while ((fatCluster = (CGCluster*)next++) != 0)
	fatCluster->innerSched()->setTarget(*mtarget->child((fatCluster->getProcId())));
    topCluster->generateSchedule();

    // Before we can fix the buffers sizes we must set the repetitions
    // count of the top cluster.
    topCluster->repetitions = 1;

    // Now fix buffer sizes
    topCluster->fixBufferSizes(1);

    return;
}

int MultiScheduler::computeSchedule(Galaxy& /*g*/) {
     return topCluster->generateSchedule();
}

void MultiScheduler :: compileRun() {
    topScheduler.compileRun();
}

void MultiScheduler :: prepareCodeGen() {
    // prepare code generation for each processing element:
    // galaxy initialize, copy schedule, and simulate the schedule.
    ParScheduler::prepareCodeGen();
}
