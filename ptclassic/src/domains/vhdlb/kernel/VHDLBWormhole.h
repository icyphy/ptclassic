#ifndef _VHDLBWormhole_h
#define _VHDLBWormhole_h 1
/**************************************************************************
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

 Programmer:  Asawaree Kalavade 
 Date of creation: 8/29/92
 Revisions:
	VHDLB EventHorizon is defined.
******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "Wormhole.h"
#include "pt_fstream.h"
#include "HLLTarget.h"
#include "VHDLBTarget.h"
#include "VHDLBPortHole.h"
#include "StringList.h"
#include "StringState.h"
#include "Block.h"
#include "EventHorizon.h"
#include "VHDLBStar.h"

	//////////////////////////////////////////
	///	class VHDLBToUniversal	
	/////////////////////////////////////////

//Input boundary of ??inVHDLB_Wormhole
class VHDLBToUniversal : public ToEventHorizon, public InVHDLBPort
{
public:
	// constructor
	VHDLBToUniversal() : ToEventHorizon(this) {}

	// redefine
	void receiveData ();	// sending data from VHDLB to domain i/f
	
	void initialize();

	void getData();

	void getIntVal(const char* uniqueName, int* intptr);

	int isItInput() const;
	int isItOutput() const;
	
	// as EventHorizon
	EventHorizon* asEH();

	/*virtual*/ Geodesic* allocateGeodesic()
	{ return ToEventHorizon::allocateGeodesic(); }

protected:
	StringList sanitize(const char* s) const;
	StringList sanitizeGhost(const char* s) const;
	int getDataFromFile();
	StringList outputName;
	StringList q1;
	StringList univName;
	istream* input;
};

	//////////////////////////////////////////
	///	class VHDLBFromUniversal	
	/////////////////////////////////////////

// Output boundary of ??inVHDLB_Wormhome
class VHDLBFromUniversal : public FromEventHorizon, public OutVHDLBPort
{
public:
	// constructor
	VHDLBFromUniversal() : FromEventHorizon(this) {} 

	// redefine
       	void sendData ();      //  sending data from domain interface to VHDLB

	void initialize();
	
	void putData();

	void putIntVal(const char* uniqueName, int* intptr);

	int isItInput() const;
        int isItOutput() const;
        
	// as EventHorizon
	EventHorizon* asEH();

	/*virtual*/ Geodesic* allocateGeodesic()
	{ return FromEventHorizon::allocateGeodesic(); }

protected:
	StringList sanitize(const char* s) const;
	StringList sanitizeGhost(const char* s) const;
	int  putDataToFile();
	int  ready();
	StringList inputName;
	StringList univName;
};
#endif
