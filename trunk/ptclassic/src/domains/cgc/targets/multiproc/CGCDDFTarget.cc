static const char file_id[] = "CGCDDFTarget.cc";
/******************************************************************
Version identification:
@(#)CGCDDFTarget.cc	1.6	12/10/92

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

// -----------------------------------------------------------------------------	
CGCDDFTarget::CGCDDFTarget(const char* name,const char* starclass,
		   const char* desc) : CGDDFTarget(name,starclass,desc) {}

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
			if (mo == 1) {
				t->incrementalAdd(ds);
			} else {
				t->writeFiring(*ds, 1);
			}
			out = "\t\t continue;\n\t}\n";
			t->getStream("code")->put(out);
		}
	}
}

void CGCDDFTarget :: endCode_For(CGTarget* t) {
	t->getStream("code")->put("\t}\n\t}\n");
}

// -----------------------------------------------------------------------------
ISA_FUNC(CGCDDFTarget,CGDDFTarget);

static CGCDDFTarget targ("default_CGCDDF","CGDDFStar",
"A DDF target for C code generation");

static KnownTarget entry(targ,"default_CGCDDF");

