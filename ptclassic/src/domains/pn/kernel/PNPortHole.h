/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	6 February 1992

    Definitions of domain-specific PortHole classes.
*/

#ifndef _MTDFConnect_h
#define _MTDFConnect_h

#ifdef __GNUG__
#pragma interface
#endif

#include "Connect.h"
class MTDFGeodesic;

class MTDFPortHole : public PortHole
{
public:
    // Class identification.
    virtual int isA(const char* className) const;

    // Constructor.
    MTDFPortHole();

    // An alias for the inherited myGeodesic member.
    MTDFGeodesic*& myGeodesic;

    // Allocate and return a MTDFGeodesic.
    virtual Geodesic* allocateGeodesic();

    /* Redefine PortHole methods because Geodesic lacks certain virtual
       functions.
    */
    void MTDFPortHole::getParticle();
    void MTDFPortHole::putParticle();
};

class InMTDFPort : public MTDFPortHole
{
public:
    // Input/output identification.
    virtual int isItInput() const;

    // Get data from the Geodesic.
    virtual void grabData();
};


class OutMTDFPort : public MTDFPortHole
{
public:
    // Input/output identification.
    virtual int isItOutput() const;

    // Put data into the Geodesic.
    virtual void sendData();
};

 
class MultiMTDFPort : public MultiPortHole
{
};

class MultiInMTDFPort : public MultiMTDFPort
{
public:
    // Input/output identification.
    virtual int isItInput() const;
 
    // Add a new physical port to the MultiPortHole list.
    virtual PortHole& newPort();
};
 
class MultiOutMTDFPort : public MultiMTDFPort
{     
public:
    // Input/output identification.
    virtual int isItOutput() const;

    // Add a new physical port to the MultiPortHole list.
    virtual PortHole& newPort();
};

#endif
