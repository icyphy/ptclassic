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
*/

/**************************************************************************
Version identification:
$Id$

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
// setCoreLimit();
    if (setHandlers((SIG_PT) signalHandler) != 0)
	returnValue = 1;
    setStrings();

    return returnValue;

}

// setCoreLimit
//
// This function sets the value of the maximum size of core file 
// allowed.                                                      

int setCoreLimit(void) 
{

#if !defined(PTHPPA)

    struct rlimit coreLimit;

    // getrlimit gets information about RLIMIT (max size of core files)
    // and places it in a rlimit struct.  Returns 0 on failure.
    if (getrlimit(RLIMIT_CORE, &coreLimit) != 0) {
        return 1;
    }

    // Set RLIMIT to max allowable value to insure that core file is generated.
    // If this were set to zero, it would prevent a core file from being made.
    coreLimit.rlim_cur = coreLimit.rlim_max;
    coreLimit.rlim_max = coreLimit.rlim_max;

    // setrlimit sets system values to the information in rlimit struct
    if (setrlimit(RLIMIT_CORE, &coreLimit) != 0) {
        return 1;
    }

#endif // PTHPPA

    return 0;  
  
}



