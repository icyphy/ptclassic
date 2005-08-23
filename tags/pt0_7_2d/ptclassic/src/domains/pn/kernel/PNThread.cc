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
/*  Version @(#)PNThread.cc	2.6 3/29/96
    Programmer:		T.M. Parks
    Date of creation:	18 February 1992
*/

static const char file_id[] = "PNThread.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "PNThread.h"
#include "DataFlowStar.h"
#include "DFPortHole.h"

extern const char PNdomainName[];

void DataFlowProcess::run()
{
    // Configure the star for dynamic execution.
    star.setDynamicExecution(TRUE);

    star.begin();
    do	// Fire the Star ad infinitum.
    {
	if (star.waitPort()) star.waitPort()->receiveData();
    } while(star.run());
}

void SyncDataFlowProcess::run()
{
    int i = 0;

    // Configure the star for dynamic execution.
    star.setDynamicExecution(TRUE);

    star.begin();
    do	// Fire the star ad infinitum.
    {
	// Wait for notification to start.
	{
	    CriticalSection region(start.monitor());
	    while (iteration <= i) start.wait();
	    i = iteration;
	}
	if (star.waitPort()) star.waitPort()->receiveData();
    } while (star.run());
}
