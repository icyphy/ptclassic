/******************************************************************
Version identification:
$Id$

@Copyright (c) 1993-%Q% The Regents of the University of California.
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

BDF C code generation!

*******************************************************************/
static const char file_id[] = "CGCBDFTarget.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCBDFTarget.h"
#include "BDFCluster.h"
#include "KnownTarget.h"
#include "CGCPortHole.h"

CGCBDFTarget::CGCBDFTarget(const char* name, const char* starclass,
			 const char* desc)
: CGCTarget(name,starclass,desc)
{
	// make loopingLevel invisible and always set.
	loopingLevel.clearAttributes(A_SETTABLE);
	loopingLevel.setInitValue("1");
}

Block* CGCBDFTarget::makeNew() const {
	LOG_NEW; return new CGCBDFTarget(name(),starType(),descriptor());
}

void CGCBDFTarget::setup() {
	if (!scheduler()) {
		StringList fname;
		fname << galaxy()->name() << ".bdf-sched";
		StringList logPath = logFilePathName(destDirectory, fname);
		delete [] schedFileName;
		schedFileName = logPath.newCopy();
		LOG_NEW; setSched(new BDFClustSched(schedFileName));
	}
	CGCTarget::setup();
}

void CGCBDFTarget :: beginIf(PortHole& cond, int truthdir,
			    int depth, int /*haveElsePart*/) {
	CGCPortHole* realCond = (CGCPortHole*)&cond;
	*defaultStream << indent(depth);
	const char* tflag = truthdir ? "" : "!";
	*defaultStream << "if (" << tflag;
	*defaultStream << realCond->getGeoName() << ") {\n";
}

void CGCBDFTarget :: beginElse(int depth) {
	*defaultStream << indent(depth);
	*defaultStream << "} else {\n";
}

void CGCBDFTarget :: endIf(int depth) {
	*defaultStream << indent(depth);
	*defaultStream << "}\n";
}

void CGCBDFTarget :: beginDoWhile(int depth) {
	*defaultStream << indent(depth);
	*defaultStream << "do {\n";
}

void CGCBDFTarget :: endDoWhile(PortHole& cond,int truthdir,int depth) {
	const char* tflag = truthdir ? "" : "!";
	CGCPortHole* realCond = (CGCPortHole*)&cond;
	*defaultStream << indent(depth);
	*defaultStream << "} while (" << tflag << realCond->getGeoName() 
	       << ");\n";
}

static CGCBDFTarget proto("bdf-CGC", "CGCStar",
			 "C code generation target with BDF code generation ability");
static KnownTarget entry(proto,"bdf-CGC");
