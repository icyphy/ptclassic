static const char file_id[] = "CGCDDFCode.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1995 Seoul National University
@Copyright (c) 1995-%Q% The Regents of the University of California.
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

 Programmer: S. Ha

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "pt_fstream.h"
#include "Error.h"
#include "CGCStar.h"
#include "KnownTarget.h"
#include "CGCDDFCode.h"
#include "CGCTarget.h"
#include "CGCGeodesic.h"
#include "CGCMultiTarget.h"

// ----------- target dependent section......

void CGCDDFCode :: startCode_Case(CGStar* s, Geodesic* gd, CGTarget* t) {
	t->switchDefaultStream(*(t->getStream("mainLoop")));
	StringList out;
	out << "\tswitch((int)";
	if (gd) {
		out << ((CGCGeodesic*) gd)->getBufName();
	} else {
		CGCPortHole* p = (CGCPortHole*) s->portWithName("output");
		out << p->getGeoName();
	}
	out << ") {\n\t case 0: {\n";
	t->getStream()->put(out);
}

void CGCDDFCode :: middleCode_Case(int i, CGTarget* t) {
	t->switchDefaultStream(*(t->getStream("mainLoop")));
	StringList out;
	out << "\tbreak; }\n\t case " << i+1 << ": {\n";
	t->getStream()->put(out);
}

void CGCDDFCode :: endCode_Case(CGTarget* t) {
	t->switchDefaultStream(*(t->getStream("mainLoop")));
	t->getStream()->put("\tbreak; }\n\t}\n");
}
	
void CGCDDFCode :: startCode_DoWhile(Geodesic* d, Geodesic* s, CGTarget* t) {
	t->switchDefaultStream(*(t->getStream("mainLoop")));
	if (d && s) {
		StringList out;
		out << "\t" << ((CGCGeodesic*) d)->getBufName() << " = "
		    << ((CGCGeodesic*) s)->getBufName() << ";\n";
		t->getStream()->put(out);
	}
	t->getStream()->put("\tdo {\n");
}

void CGCDDFCode :: endCode_DoWhile(CGStar* s, Geodesic* gd, CGTarget* t) {
	t->switchDefaultStream(*(t->getStream("mainLoop")));
	StringList out;
	out << "\t} while ((int)";
	if (gd) {
		out << ((CGCGeodesic*) gd)->getBufName();
	} else {
		CGCPortHole* p = (CGCPortHole*) s->portWithName("output");
		out << p->getGeoName();
	}
	out << ");\n";
	t->getStream()->put(out);
}
	
void CGCDDFCode :: startCode_For(CGStar* s, Geodesic* gd, CGTarget* t) {
	t->switchDefaultStream(*(t->getStream("mainLoop")));
	StringList out;
	out << "\t{ int i;\n\t for (i = 0; i < ";
	if (gd) {
		out << ((CGCGeodesic*) gd)->getBufName();
	} else {
		CGCPortHole* p = (CGCPortHole*) s->portWithName("output");
		out << p->getGeoName();
	}
	out << "; i++) {\n";
	t->getStream()->put(out);
}

void CGCDDFCode :: middleCode_For(CGStar* rv, CGStar* ds, int k, 
				    int mo, CGTarget* t) {
	t->switchDefaultStream(*(t->getStream("mainLoop")));
	// for intercycle parallelism, check modulo.
	StringList out;
	if (k > 1) {
		if (!rv) {
			out << "\t\t if (i % " << k << " != " 
			    << mo << ") continue;\n";
			t->getStream()->put(out);
		} else {
			out << "\t\t"; 
			if (mo != 1) out << " else";
 			out << " if (i % " << k << " == " << mo << ") {\n";
			t->getStream()->put(out);

			t->incrementalAdd(rv);
			int zz = 0;
			if (mo == 1) zz = 1;
			t->incrementalAdd(ds, zz);

			out = "\t\t continue;\n\t}\n";
			t->getStream()->put(out);
		}
	}
}

void CGCDDFCode :: endCode_For(CGTarget* t) {
	t->switchDefaultStream(*(t->getStream("mainLoop")));
	t->getStream()->put("\t}\n\t}\n");
}

const char* whichType(DataType);

void CGCDDFCode :: startCode_Recur(Geodesic* arg, PortHole* p,
	const char* fName, CGTarget* ct) {

	CGCTarget* t = (CGCTarget*) ct;

	// First step.
	// Switch some code streams of the target and save that code streams.
	CodeStream* temp;

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

	saveCode = t->removeStream("mainLoop");
	temp = new CodeStream;
	temp->initialize();
	t->putStream("mainLoop", temp);
	oldDefault = &(t->switchDefaultStream(*temp));;

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

void CGCDDFCode :: middleCode_Recur(Geodesic* selfGeo, Geodesic* gd, 
					const char* fName, CGTarget* ct) {
	CGCTarget* t = (CGCTarget*) ct;
	t->switchDefaultStream(*(t->getStream("mainLoop")));
	
	StringList out = "\t";
	if (selfGeo) {
		out << ((CGCGeodesic*) selfGeo)->getBufName() << " = ";
	} 
	out << fName << "(";
	if (selfGeo) {
		out << ((CGCGeodesic*) gd)->getBufName();
	}
	out << ");\n";
	t->getStream()->put(out);
}

void CGCDDFCode :: endCode_Recur(Geodesic* gd, const char* fName, 
						CGTarget* ct) {
	CGCTarget* t = (CGCTarget*) ct;

	StringList out;
		
	// Form the function and delete the temporary code streams.
	CodeStream* temp;
	
	// function name first
	out << funcName;

	// declaration.
	temp = t->removeStream("mainDecls");
	out << *temp;
	LOG_DEL; delete temp;

	// initialization.
	temp = t->removeStream("mainInit");
	out << *temp;
	LOG_DEL; delete temp;

	// body
	temp = t->removeStream("mainLoop");
	out << *temp;
	LOG_DEL; delete temp;

	// closure
	temp = t->removeStream("mainClose");
	// do not add wrap code inside the recursion function.
	// out << *temp;
	LOG_DEL; delete temp;

	if (gd) {
		out << "\t return ";
		out << ((CGCGeodesic*) gd)->getBufName() << ";\n";
	}
	out << "}\n";

	t->getStream("procedure")->put(out, fName);

	// restore the code streams
	t->putStream("mainInit", saveMainInit);
	t->putStream("mainDecls", saveMainDecls);
	t->putStream("mainClose", saveMainClose);
	t->putStream("mainLoop", saveCode);
	t->switchDefaultStream(*oldDefault);
}

void CGCDDFCode :: prepCode(MultiTarget* refT, Profile* pf,
			int numP, int numChunk) {
	CGCMultiTarget* t = (CGCMultiTarget*) refT;
	t->prepCode(pf, numP, numChunk);
}

void CGCDDFCode :: signalCopy(int flag) {
	CGCMultiTarget* t = (CGCMultiTarget*) owner;
	if (t) t->signalCopy(flag);
}

/////////////////////////////////////////
// Utilities
/////////////////////////////////////////
 
const char* whichType(DataType s) {
	if ((strcmp(s, INT) == 0) || (strcmp(s, "INTARRAY") == 0))
		return "int";
	else if ((strcmp(s, COMPLEX) == 0) || (strcmp(s, "COMPLEXARRAY") == 0))
		return "complex";
	else if (strcmp(s, "STRING") == 0) return "char*";
		return "double";
}

