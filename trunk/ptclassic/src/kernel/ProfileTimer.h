/* 
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

   Version: $Id$
   Programmer:  Jose Luis Pino
   Date of creation:  3/27/95

*/

#ifndef _ProfileTimer_h
#define _ProfileTimer_h

#ifdef __GNUG__
#pragma interface
#endif

#include "TimeVal.h"

class ProfileTimer {
public:
    ProfileTimer();

    // Reset startTime.
    void reset();

    // Elapsed CPU time in seconds since the last reset.  The time is equal
    // to the time the Ptolemy process is executing time plus the time and 
    // when the UNIX system is running on behalf of Ptolemy.
    TimeVal elapsedCPUTime() const;

private:
    // When the ProfileTimer was started.
    TimeVal startTime;

    // Have we setup the profile timer?  We only do this
    // once for an entire Ptolemy simulation.  Things might
    // have to change if we ever allow multithreading.
    static int setupFlag;

    // Read the system profile clock.
    TimeVal timeOfProfile() const;

};

#endif
