#ifndef _VHDLBTarget_h
#define _VHDLBTarget_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: Edward A. Lee

 Basic target for VHDLB code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#define  CHAR15  15
#define  CHAR10  10

#include "HLLTarget.h"
#include "IntState.h"
#include "StringState.h"
#include "InterpGalaxy.h"
#include "Galaxy.h"
#include "DynamicGalaxy.h"
#include "NamedObj.h"

class VHDLBPortHole;

class VHDLBTarget : public HLLTarget {
public:
	VHDLBTarget(const char* name, const char* starclass, const char* desc);
	// copy constructor
	VHDLBTarget(const VHDLBTarget&);
	Block* makeNew() const;
	void headerCode();
	virtual void setup();
	int run();
	void wrapup();

	// make public this method
	StringList correctName(NamedObj& p) {return  sanitizedFullName(p); }

	// Return the data type suitable for VHDL from the ptlang
	// standart data type string.
	StringList translateType(const char* type);

	// Return the portHole direction.
	StringList direction(const GenericPort* port);

// public?
// 	put this a diff name to generate VHDL compatible code
	StringList vhdlCode;
	StringList galaxyList;
	StringList univName;
	StringList runOptions;
	StringList changeOptions;
	StringList removeOldFiles;

protected:
	// Redefine to use names acceptable to VHDL compiler
	StringList sanitizedFullName(const NamedObj &b) const;

	// Return a name composed of the name of the parent object,
	// an underscore, and the name of the object.
	// If there is no parent, use "XX".
	virtual StringList sanitizedShortName(const NamedObj &b) const;

	char *schedFileName;
	CodeStream include;
	CodeStream mainDeclarations;
	CodeStream mainInitialization;
	CodeStream mainCode;
	CodeStream comCode;
	// code generation init routine; generate initCode
	int codeGenInit();

	// give a unique name for a galaxy.
        int galId;


private:
	StringList includeFiles;
	StringList globalDecls;
	StringList sectionComment(const StringList s);
	int galFunctionDef(Galaxy& galaxy);

	int starDataStruct(Block& block, int level=0);
	void setGeoNames(Galaxy& galaxy);

};
#endif
