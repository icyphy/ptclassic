#ifndef _VHDLBWormhole_h
#define _VHDLBWormhole_h 1
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

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

protected:
	StringList sanitize(const char* s) const;
	StringList sanitizeGhost(const char* s) const;
	int  putDataToFile();
	int  ready();
	StringList inputName;
	StringList univName;
};
#endif
