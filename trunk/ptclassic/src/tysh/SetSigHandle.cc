/* 
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
*/

/**************************************************************************
Version identification:
@(#)SetSigHandle.cc	1.19	2/25/96

Author: Joel R. King

Sets up the signal handlers for Tycho.

**************************************************************************/

// SigHandle.h defines SIG_PT
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "compat.h"
#include "SigHandle.h"
#include "SetSigHandle.h"

// setSignalHandlers
//
// This function sets up the signal handlers.

int setSignalHandlers(void) 
{

#if !defined(PTHPPA) && !defined(PTSOL2) && !defined(PTSUN4) \
&& !defined(PTIRIX5)

    // We only handle the above four cases. For the others do nothing.
    // FIX-ME: handle other UNIX platforms and minimum ANSI C signal set
    // for PC and MAC platforms.
    return 0; 

#endif

    int returnValue = 0;
    // FIXME: ptSignal returns the value of the previous function
    // function pointer that handled the signal we are setting our
    // function to handle. What we really want is a return value that
    // lets us know if the handler was set successfully, and then 
    // base our return value (nonzero if failed to set) on this.
    // Unforunately this requires a change to the current implementation
    // of ptSignal, or a another function.
    setHandlers((SIG_PT) signalHandler);	
    setStrings();

    return returnValue;

}




