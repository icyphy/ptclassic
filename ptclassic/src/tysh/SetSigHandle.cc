/* 
Copyright (c) 1990-1995 The Regents of the University of California.
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
@(#)SetSigHandle.cc	

Author: Joel R. King

Sets up the signal handlers for Tycho.

**************************************************************************/

#include "SetSigHandle.h"
#include "SigHandle.h"

/****************************************************************************/

/***** This function sets up the signal handlers and, makes sure that *******/
/***** RLIMIT is not 0 (if in development mode) which would prevent a *******/
/***** core file from being generated. The environmental variable     *******/
/***** PT_DEVELOP when set to a non-zero value, or not set at all,    *******/
/***** will cause the core to be dumped, and the debugger to be run.  *******/

int 
setSignalHandlers(void) 
{

    char *isDevelop; 
    int returnValue = 0;

    isDevelop = getenv("PT_DEVELOP"); /* getenv(char *) returns a pointer to*/
                                      /* environmental variable or returns  */
                                      /* NULL if it does not exist. This is */
                                      /* used to get value of PT_DEVELOP if */
                                      /* it exists.                         */
         
    if (isDevelop == 0 || isDevelop[0] == '0') 
    {
        if (setCoreLimitRelease() != 0) 
            returnValue = 1; 
        if (setReleaseHandlers() != 0)
	    returnValue = 2;
    }
    else
    {
        if (setCoreLimitDebug() != 0) 
            returnValue = 3; 
        if (setDebugHandlers() != 0)
	    returnValue = 4;
    }

    return returnValue;

}

/****************************************************************************/

/****** This function sets the value of the maximum size of core file *******/
/****** allowed.                                                      *******/

int
setCoreLimitDebug(void) 
{

    struct rlimit coreLimit;

    if (getrlimit(RLIMIT_CORE, &coreLimit) != 0) 
    {             /* getrlimit gets information about RLIMIT (max size      */
        return 1; /* of core) and places it in a rlimit struct. Returns 0   */
    }             /* on a failure.                                          */

    coreLimit.rlim_cur = coreLimit.rlim_max; /* Set RLIMIT to max allowable */
                                             /* value, to insure that core  */
                                             /* file is generated. If this  */
                                             /* was set to zero it would    */
                                             /* prevent a core file from    */
                                             /* being made.                 */

    if (setrlimit(RLIMIT_CORE, &coreLimit) != 0) 
    {             /* setrlimit sets system values to the information in     */
        return 1; /* rlimit struct.                                         */
    }

    return 0;  
  
}

/****************************************************************************/


/****** This function sets the value of the maximum size of core file *******/
/****** allowed.                                                      *******/

int
setCoreLimitRelease(void) 
{

    struct rlimit coreLimit;

    coreLimit.rlim_cur = 0; /* Set RLIMIT 0. This prevents core file from   */
                            /* being generated. This is included just in    */
                            /* case an error occurs in the setting of the   */
                            /* signal handlers, and the user continues.     */
                            /* In normal operation however any signals that */
                            /* could cause a core dump are intercepted so   */
                            /* that a core file would never be generated.   */

    if (setrlimit(RLIMIT_CORE, &coreLimit) != 0) 
    {             /* setrlimit sets system values to the information in     */
        return 1; /* rlimit struct.                                         */
    }

    return 0;  
  
}

/****************************************************************************/

