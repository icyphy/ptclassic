static const char file_id[] = "CGCBDFTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1993 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmer: J. Buck

BDF C code generation!

*******************************************************************/

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
		writeDirectoryName(destDirectory);
		schedFileName = writeFileName(fname);
		LOG_NEW; setSched(new BDFClustSched(schedFileName));
	}
	CGCTarget::setup();
}

void CGCBDFTarget :: beginIf(PortHole& cond,int truthdir,
			    int depth,int haveElsePart) {
	CGCPortHole* realCond = (CGCPortHole*)&cond;
	myCode << indent(depth);
	const char* tflag = truthdir ? "" : "!";
	myCode << "if (" << tflag;
	myCode << realCond->getGeoName() << ") {\n";
}

void CGCBDFTarget :: beginElse(int depth) {
	myCode << indent(depth);
	myCode << "} else {\n";
}

void CGCBDFTarget :: endIf(int depth) {
	myCode << indent(depth);
	myCode << "}\n";
}

void CGCBDFTarget :: beginDoWhile(int depth) {
	myCode << indent(depth);
	myCode << "do {\n";
}

void CGCBDFTarget :: endDoWhile(PortHole& cond,int truthdir,int depth) {
	const char* tflag = truthdir ? "" : "!";
	CGCPortHole* realCond = (CGCPortHole*)&cond;
	myCode << indent(depth);
	myCode << "} while (" << tflag << realCond->getGeoName() 
	       << ");\n";
}

static CGCBDFTarget proto("bdf-CGC", "CGCStar",
			 "C code generation target with BDF code generation ability");
static KnownTarget entry(proto,"bdf-CGC");
