static const char file_id[] = "VHDLBWormhole.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee, Michael C. Williamson

Code for functions declared in VHDLBWormhole.h

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Wormhole.h"
#include "Error.h"
#include "CircularBuffer.h"
#include "StringList.h"
#include "StringState.h"
#include "Plasma.h"
#include "PortHole.h"
#include "VHDLBWormhole.h"
#include "VHDLBPortHole.h"
#include "VHDLBStar.h"
#include "VHDLBTarget.h"
 
/**************************************************************************

	methods for VHDLBToUniversal

**************************************************************************/

///////////////////////////////////////////////////////////////
//		VHDLBToUniversal :: receiveData()
///////////////////////////////////////////////////////////////

void VHDLBToUniversal :: receiveData()
{
	// Check whether it is an input or output.
	if (VHDLBToUniversal::isItInput()) 
	{
		// VHDLB domain on outside
/*
		Error::abortRun(*this, "lies in input port",
				"(there is no VHDLB wormhole, sorry1)");
*/
	} 
	else 
	{
		// VHDLB is on the inside of the other (SDF?) domain
		Error::abortRun(*this, "lies in output port",
				"(there is no VHDLB wormhole, sorry2)");
	}
}

///////////////////////////////////////////////////////////////
//		VHDLBToUniversal :: initialize()
///////////////////////////////////////////////////////////////

void VHDLBToUniversal :: initialize()
{
	InVHDLBPort :: initialize();
        ToEventHorizon :: initialize();         // for EventHorizon class
}

//
// VHDLBToUniversal :: getData()
//

void VHDLBToUniversal :: getData()
{
}

/* leave virtual for now
//
// VHDLBToUniversal :: transferData()
//

void VHDLBToUniversal :: transferData()
{
}
*/

///////////////////////////////////////////////////////////////
//		isItIn(Out)Put
///////////////////////////////////////////////////////////////

int VHDLBToUniversal :: isItInput() const
        { return EventHorizon :: isItInput(); }
int VHDLBToUniversal :: isItOutput() const
 	{ return EventHorizon :: isItOutput(); }

EventHorizon* VHDLBToUniversal :: asEH() { return this; }

/**************************************************************************

	methods for VHDLBFromUniversal

**************************************************************************/

///////////////////////////////////////////////////////////////
//		VHDLBFromUniversal :: sendData()
///////////////////////////////////////////////////////////////

void VHDLBFromUniversal :: sendData()
{
	transferData();

	if (VHDLBFromUniversal::isItOutput()) {
	        // VHDLB is on the inside of the other (SDF?) domain
/*
		Error::abortRun(*this, "lies in output port",
				"(there is no VHDLB wormhole, sorry3)");
*/
	}
	else 
	{
	        // VHDLB domain on outside
		Error::abortRun(*this, "lies in input port",
				"(there is no VHDLB wormhole, sorry4)");
	}
}

///////////////////////////////////////////////////////////////
//		VHDLBFromUniversal :: initialize()
///////////////////////////////////////////////////////////////

void VHDLBFromUniversal :: initialize()
{
	OutVHDLBPort :: initialize();
        FromEventHorizon :: initialize();         
}	

//
// VHDLBFromUniversal :: putData()
//

void VHDLBFromUniversal :: putData()
{
}

/* leave virtual for now
//
// VHDLBFromUniversal :: transferData()
//

void VHDLBFromUniversal :: transferData()
{
}
*/

///////////////////////////////////////////////////////////////
//		VHDLBFromUniversal :: ready()
///////////////////////////////////////////////////////////////

int VHDLBFromUniversal :: ready() {
	// need for multirate stuff...not sure

        DFPortHole *pFar = (DFPortHole*)far();
        int target = pFar->numXfer();
        //int target = pFar->numXfer() * pFar->parentReps();
        if (numTokens() >= target) {
                // yes, enough tokens
        	return TRUE;
	}
        return FALSE;
}

///////////////////////////////////////////////////////////////
//		isItIn(Out)Put
///////////////////////////////////////////////////////////////

int VHDLBFromUniversal :: isItInput() const
        { return EventHorizon :: isItInput(); }
int VHDLBFromUniversal :: isItOutput() const
        { return EventHorizon :: isItOutput(); }

EventHorizon* VHDLBFromUniversal :: asEH() { return this; }
