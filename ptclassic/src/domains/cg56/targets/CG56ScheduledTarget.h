#ifndef _CG56ScheduledTarget_h
#define _CG56ScheduledTarget_h 1
/******************************************************************
Version identification:
$Id$

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

 Programmer: Asawaree Kalavade
A taret that generates code according to a predefined schedule.
This is currently used in the Design Assistant.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CG56Target.h"
#include "StringState.h"
#include "CG56Star.h"

class CG56ScheduledTarget : public CG56Target {
public:
	CG56ScheduledTarget (const char*, const char*);
	CG56ScheduledTarget (const CG56ScheduledTarget&);

	Block* makeNew() const;
	/*virtual*/ int isA(const char*) const;
	void setup();
	CG56Star* starWithFullName(const char* name);

private:
	StringState	schedLoadFile;
	void initStates();

protected:
	/* virtual */ void headerCode();
};


#endif
