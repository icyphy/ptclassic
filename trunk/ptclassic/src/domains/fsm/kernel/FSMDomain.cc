static const char file_id[] = "FSMDomain.cc";
/**********************************************************************
Version identification:
$Id$

@Copyright (c) 1996-%Q% The Regents of the University of California.
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


 Programmer:  Bilung Lee
 Date of creation: 3/2/96

 WARNING -- XDomain.ccP is a template file that is used to generate
 domain description modules.  If the name of this file is not XDomain.ccP,
 DO NOT EDIT IT!!!

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the FSM domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "FSMTarget.h"
#include "KnownTarget.h"
#include "FSMScheduler.h"
#include "FSMWormhole.h"

extern const char FSMdomainName[] = "FSM";

class FSMDomain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		LOG_NEW; return *new FSMWormhole(innerGal,innerTarget);
	}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { LOG_NEW; return *new FSMfromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { LOG_NEW; return *new FSMtoUniversal;}

	// constructor
	FSMDomain() : Domain("FSM") {}
};

// declare a prototype
static FSMDomain proto;

static FSMTarget defaultFSMtarget;
static KnownTarget entry(defaultFSMtarget,"default-FSM");
