/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	6 February 1992

    Definitions of domain-specific PortHole classes.
*/

#ifndef _MTDFPortHole_h
#define _MTDFPortHole_h

#ifdef __GNUG__
#pragma interface
#endif

#include "PortHole.h"
class MTDFGeodesic;

class MTDFPortHole : public PortHole
{
public:
    // Class identification.
    virtual int isA(const char* className) const;

    // Constructor.
    MTDFPortHole::MTDFPortHole()
	: myGeodesic((MTDFGeodesic*&)PortHole::myGeodesic) {}

    // Allocate and return a MTDFGeodesic.
    /* virtual */ Geodesic* allocateGeodesic();

    /* Redefine PortHole methods because Geodesic lacks certain virtual
       functions.
    */
    void MTDFPortHole::getParticle();
    void MTDFPortHole::putParticle();

protected:
    // An alias for the inherited myGeodesic member.
    MTDFGeodesic*& myGeodesic;
};

class InMTDFPort : public MTDFPortHole
{
public:
    // Input/output identification.
    /* virtual */ int isItInput() const;

    // Get data from the Geodesic.
    /* virtual */ void receiveData();
};


class OutMTDFPort : public MTDFPortHole
{
public:
    // Input/output identification.
    /* virtual */ int isItOutput() const;

    // Update buffer pointer (for % operator) and clear old Particles.
    /* virtual */ void receiveData();

    // Put data into the Geodesic.
    /* virtual */ void sendData();
};

 
class MultiMTDFPort : public MultiPortHole
{
};

class MultiInMTDFPort : public MultiMTDFPort
{
public:
    // Input/output identification.
    /* virtual */ int isItInput() const;
 
    // Add a new physical port to the MultiPortHole list.
    /* virtual */ PortHole& newPort();
};
 
class MultiOutMTDFPort : public MultiMTDFPort
{     
public:
    // Input/output identification.
    /* virtual */ int isItOutput() const;

    // Add a new physical port to the MultiPortHole list.
    /* virtual */ PortHole& newPort();
};

#endif
