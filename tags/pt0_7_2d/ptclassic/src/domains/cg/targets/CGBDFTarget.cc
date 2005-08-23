/******************************************************************
Version identification:
@(#)CGBDFTarget.cc	1.9	7/30/96

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

 Programmer: J. Buck

BDF code generation!

*******************************************************************/
static const char file_id[] = "CGBDFTarget.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGBDFTarget.h"
#include "BDFCluster.h"
#include "KnownTarget.h"

CGBDFTarget::CGBDFTarget(const char* name, const char* starclass,
			 const char* desc, const char* assocDomain) :
CGTarget(name,starclass,desc,assocDomain)
{
	// make loopingLevel invisible
	loopingLevel.clearAttributes(A_SETTABLE);
}

Block* CGBDFTarget::makeNew() const {
	LOG_NEW; return new CGBDFTarget(name(),starType(),descriptor(),
					getAssociatedDomain());
}

void CGBDFTarget::setup() {
	if (!scheduler()) {
		delete [] schedFileName;
		// Full path name of the log file
		StringList logPath =
			logFilePathName(destDirectory, "schedule.log");
		schedFileName = logPath.newCopy();
		LOG_NEW; setSched(new BDFClustSched(schedFileName));
	}
	CGTarget::setup();
}

void CGBDFTarget :: beginIf(PortHole& cond,int truthdir,
			    int depth, int /*haveElsePart*/) {
	const char* tflag = truthdir ? "" : "!";
	myCode << "if (" << tflag << cond.fullName() << ") { /* depth = "
	       << depth << " */\n";
}

void CGBDFTarget :: beginElse(int /*depth*/) {
	myCode << "} else {\n";
}

void CGBDFTarget :: endIf(int depth) {
	myCode << "} /* end depth " << depth << " */\n";
}

void CGBDFTarget :: beginDoWhile(int depth) {
	myCode << "do { /* depth " << depth << " */\n";
}

void CGBDFTarget :: endDoWhile(PortHole& cond,int truthdir,int depth) {
	const char* tflag = truthdir ? "" : "!";
	myCode << "} while (" << tflag << cond.fullName()
	       << ") /* end depth " << depth << " */\n";
}

static CGBDFTarget proto("bdf-CG", "CGStar",
			 "CG target with BDF code generation ability");
static KnownTarget entry(proto,"bdf-CG");
