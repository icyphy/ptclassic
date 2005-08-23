#ifndef _HierScheduler_h
#define _HierScheduler_h
#ifdef __GNUG__
#pragma interface
#endif


/*****************************************************************
Version identification:
@(#)HierScheduler.h	1.10	1/1/96

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

*****************************************************************/

#include "ParScheduler.h"
#include "DynamicGalaxy.h"

class HierScheduler : public ParScheduler {
public:
    HierScheduler(MultiTarget* t, const char* log, ParScheduler& top):
    ParScheduler(t, log), topScheduler(top){};

    ~HierScheduler();

    /*virtual*/ void setup();

    // Pass through functions
    
    /*virtual*/ int run();

    /*virtual*/ ParProcessors* setUpProcs(int);

    /*virtual*/ void setStopTime(double);
    
    /*virtual*/ double getStopTime();

    /*virtual*/ StringList displaySchedule();

    /*virtual*/ void compileRun();

    /*virtual*/ int dagNodes() const;

    /*virtual*/ void writeGantt(ostream& o) { topScheduler.writeGantt(o); }

    /*virtual*/ ParGraph* myEG() {return topScheduler.myEG();}

private:
    DynamicGalaxy wormholes;
    ParScheduler& topScheduler;
    int sdfStars;
};

#endif
