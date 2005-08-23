#ifndef _ShadowTarget_h
#define _ShadowTarget_h
#ifdef __GNUG__
#pragma interface
#endif


/*****************************************************************
Version identification:
@(#)ShadowTarget.h	1.2	1/1/96

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

#include "Target.h"

class ShadowTarget : public Target {
public:
    ShadowTarget(const char* n = "ShadowTarget",const char* starClass = "",
		 const char* desc = "");

    // shouldn't we just promote the Target::setSched method to public?
    void setScheduler(Scheduler* sched) { setSched(sched); }

protected:
    // overload so that stars in the Target's galaxy are not pointing
    // to itself
    /*virtual*/ int galaxySetup();
};

#endif
