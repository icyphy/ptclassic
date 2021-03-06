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
/*  Version @(#)PNTarget.cc	2.9 07/30/96
    Author:	T. M. Parks
    Created:	2 April 1992

*/

static const char file_id[] = "PNTarget.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "Galaxy.h"
#include "PNTarget.h"
#include "PNScheduler.h"

// Defined in PNDomain.cc
extern const char PNdomainName[];

// Constructor.
PNTarget::PNTarget() : Target("default-PN", "DataFlowStar",
	"Schedule dataflow systems as process networks.",
	PNdomainName)
{
}

// Destructor.
PNTarget::~PNTarget()
{
    delSched();
}

// Make a new PNTarget object.
Block* PNTarget::makeNew() const
{
    LOG_NEW; return new PNTarget;
}

// Initialization.
void PNTarget::setup()
{
    LOG_NEW; setSched(new PNScheduler);
    Target::setup();
}

void PNTarget::wrapup()
{
    Target::wrapup();
    delSched();
}
