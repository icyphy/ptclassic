#ifndef _MultiScheduler_h
#define _MultiScheduler_h
#ifdef __GNUG__
#pragma interface
#endif


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
				ov			COPYRIGHTENDKEY

Programmer: Jose Luis Pino

*****************************************************************/

#include "ParScheduler.h"
#include "CGCluster.h"

class MultiScheduler : public ParScheduler {
public:
    MultiScheduler(MultiTarget* t, const char* log, ParScheduler& top):
    ParScheduler(t, log), topScheduler(top) {};

    ~MultiScheduler() { LOG_DEL; delete topCluster; }

    /*virtual*/ void setup();

    /*virtual*/ int computeSchedule(Galaxy&);

    // Pass through functions
    
    /*virtual*/ int run() { return topCluster->run(); }

    /*virtual*/ void setUpProcs(int num) { topScheduler.setUpProcs(num); }
    /*virtual*/ void setStopTime(double limit) {
	topCluster->setStopTime(limit);
    }
    
    /*virtual*/ double getStopTime() {
	return topCluster->getStopTime();
    }

    /*virtual*/ StringList displaySchedule() {
	StringList schedule;
	if (topCluster) schedule << topCluster->displaySchedule();
	return schedule;
    }

    /*virtual*/ void compileRun();

    /*virtual*/ void prepareCodeGen();

    /*virtual*/ int dagNodes() const;

private:
    CGCluster* topCluster;
    ParScheduler& topScheduler;
};

#endif
