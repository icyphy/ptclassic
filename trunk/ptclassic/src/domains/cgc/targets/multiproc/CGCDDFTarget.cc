static const char file_id[] = "CGCDDFTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

 Programmer: S. Ha

 A DDF multiTarget for CGC domain

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "pt_fstream.h"
#include "Error.h"
#include "CGDisplay.h"
#include "CGCStar.h"
#include "KnownTarget.h"
#include "CGCDDFTarget.h"
#include "CGCTarget.h"
#include "CGCGeodesic.h"
#include "CGCMultiTarget.h"

// -----------------------------------------------------------------------------	
CGCDDFTarget::CGCDDFTarget(const char* name,const char* starclass,
		   const char* desc) : CGDDFTarget(name,starclass,desc) { }

// -----------------------------------------------------------------------------
Block* CGCDDFTarget :: makeNew() const {
	LOG_NEW; return new CGCDDFTarget(name(),starType(),descriptor());
}

// ----------- target dependent section......

void CGCDDFTarget :: startCode_Case(CGStar* s, Geodesic* gd, CGTarget* t) {
	StringList out;
	out << "\tswitch((int)";
	if (gd) {
		out << ((CGCGeodesic*) gd)->getBufName();
	} else {
		CGCPortHole* p = (CGCPortHole*) s->portWithName("output");
		out << p->getGeoName();
	}
	out << ") {\n\t case 0: {\n";
	t->getStream("code")->put(out);
}

void CGCDDFTarget :: middleCode_Case(int i, CGTarget* t) {
	StringList out;
	out << "\tbreak; }\n\t case " << i+1 << ": {\n";
	t->getStream("code")->put(out);
}

void CGCDDFTarget :: endCode_Case(CGTarget* t) {
	t->getStream("code")->put("\tbreak; }\n\t}\n");
}
	
void CGCDDFTarget :: startCode_DoWhile(Geodesic* d, Geodesic* s, CGTarget* t) {
	if (d && s) {
		StringList out;
		out << "\t" << ((CGCGeodesic*) d)->getBufName() << " = "
		    << ((CGCGeodesic*) s)->getBufName() << ";\n";
		t->getStream("code")->put(out);
	}
	t->getStream("code")->put("\tdo {\n");
}

void CGCDDFTarget :: endCode_DoWhile(CGStar* s, Geodesic* gd, CGTarget* t) {
	StringList out;
	out << "\t} while ((int)";
	if (gd) {
		out << ((CGCGeodesic*) gd)->getBufName();
	} else {
		CGCPortHole* p = (CGCPortHole*) s->portWithName("output");
		out << p->getGeoName();
	}
	out << ");\n";
	t->getStream("code")->put(out);
}
	
void CGCDDFTarget :: startCode_For(CGStar* s, Geodesic* gd, CGTarget* t) {
	StringList out;
	out << "\t{ int i;\n\t for (i = 0; i < ";
	if (gd) {
		out << ((CGCGeodesic*) gd)->getBufName();
	} else {
		CGCPortHole* p = (CGCPortHole*) s->portWithName("output");
		out << p->getGeoName();
	}
	out << "; i++) {\n";
	t->getStream("code")->put(out);
}

void CGCDDFTarget :: middleCode_For(CGStar* rv, CGStar* ds, int k, 
				    int mo, CGTarget* t) {

	// for intercycle parallelism, check modulo.
	StringList out;
	if (k > 1) {
		if (!rv) {
			out << "\t\t if (i % " << k << " != " 
			    << mo << ") continue;\n";
			t->getStream("code")->put(out);
		} else {
			out << "\t\t"; 
			if (mo != 1) out << " else";
 			out << " if (i % " << k << " == " << mo << ") {\n";
			t->getStream("code")->put(out);

			t->incrementalAdd(rv);
			int zz = 0;
			if (mo == 1) zz = 1;
			t->incrementalAdd(ds, zz);

			out = "\t\t continue;\n\t}\n";
			t->getStream("code")->put(out);
		}
	}
}

void CGCDDFTarget :: endCode_For(CGTarget* t) {
	t->getStream("code")->put("\t}\n\t}\n");
}

extern const char* whichType(DataType);

void CGCDDFTarget :: startCode_Recur(Geodesic* arg, PortHole* p,
	const char* fName, CGTarget* ct) {

	CGCTarget* t = (CGCTarget*) ct;

	// First step.
	// Switch some code streams of the target and save that code streams.
	saveGalStruct = t->removeStream("galStruct");
	CodeStream* temp = new CodeStream;
	temp->initialize();
	t->putStream("galStruct", temp);

	saveMainInit = t->removeStream("mainInit");
	temp = new CodeStream;
	temp->initialize();
	t->putStream("mainInit", temp);

	saveMainDecls = t->removeStream("mainDecls");
	temp = new CodeStream;
	temp->initialize();
	t->putStream("mainDecls", temp);

	saveMainClose = t->removeStream("mainClose");
	temp = new CodeStream;
	temp->initialize();
	t->putStream("mainClose", temp);

	saveCode = t->removeStream("code");
	temp = new CodeStream;
	temp->initialize();
	t->putStream("code", temp);

	// function name.
	funcName.initialize();
	if (p) {
		funcName << whichType(p->resolvedType());
	} else {
		funcName << "void";
	}
	funcName << " " << fName << "(";
	if (p) {
		funcName << ((CGCGeodesic*) arg)->getBufName();
	} 
	funcName << ")\n";
	if (p) {
		funcName << whichType(arg->sourcePort()->resolvedType());
		funcName << " " << ((CGCGeodesic*) arg)->getBufName(); 
		funcName << ";\n";
	}
	funcName << "{";
}

void CGCDDFTarget :: middleCode_Recur(Geodesic* selfGeo, Geodesic* gd, 
					const char* fName, CGTarget* ct) {
	CGCTarget* t = (CGCTarget*) ct;

	StringList out = "\t";
	if (selfGeo) {
		out << ((CGCGeodesic*) selfGeo)->getBufName() << " = ";
	} 
	out << fName << "(";
	if (selfGeo) {
		out << ((CGCGeodesic*) gd)->getBufName();
	}
	out << ");\n";
	t->getStream("code")->put(out);
}

void CGCDDFTarget :: endCode_Recur(Geodesic* gd, const char* fName, 
						CGTarget* ct) {
	CGCTarget* t = (CGCTarget*) ct;

	StringList out;
		
	// Form the function and delete the temporary code streams.
	CodeStream* temp;
	
	// function name first
	out << funcName;

	// declaration.
	temp = t->removeStream("galStruct");
	out << *temp;
	LOG_DEL; delete temp;
	temp->initialize();

	temp = t->removeStream("mainDecls");
	out << *temp;
	LOG_DEL; delete temp;
	temp->initialize();

	// initialization.
	temp = t->removeStream("mainInit");
	out << *temp;
	LOG_DEL; delete temp;
	temp->initialize();

	// body
	temp = t->removeStream("code");
	out << *temp;
	LOG_DEL; delete temp;
	temp->initialize();

	// closure
	temp = t->removeStream("mainClose");
	// do not add wrap code inside the recursion function.
	// out << *temp;
	LOG_DEL; delete temp;
	temp->initialize();

	if (gd) {
		out << "\t return ";
		out << ((CGCGeodesic*) gd)->getBufName() << ";\n";
	}
	out << "}\n";

	t->getStream("procedure")->put(out, fName);

	// restore the code streams
	t->putStream("galStruct", saveGalStruct);
	t->putStream("mainInit", saveMainInit);
	t->putStream("mainDecls", saveMainDecls);
	t->putStream("mainClose", saveMainClose);
	t->putStream("code", saveCode);
}

void CGCDDFTarget :: prepCode(MultiTarget* refT, Profile* pf,
			int numP, int numChunk) {
	CGCMultiTarget* t = (CGCMultiTarget*) refT;
	t->prepCode(pf, numP, numChunk);
}

void CGCDDFTarget :: signalCopy(int flag) {
	CGCMultiTarget* t = (CGCMultiTarget*) inheritFrom();
	if (t) t->signalCopy(flag);
}

// -----------------------------------------------------------------------------
ISA_FUNC(CGCDDFTarget,CGDDFTarget);

static CGCDDFTarget targ("default_CGCDDF","CGDDFStar",
"A DDF target for C code generation");

static KnownTarget entry(targ,"default_CGCDDF");

