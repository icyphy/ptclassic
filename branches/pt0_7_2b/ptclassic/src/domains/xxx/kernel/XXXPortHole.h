/*  Version $Id$

    Copyright 1991 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	5 January 1991

    Definitions of domain-specific PortHole classes.
*/

#ifndef _XXXConnect_h
#define _XXXConnect_h

#ifdef __GNUG__
#pragma once
#pragma interface
#endif

#include "Connect.h"

class XXXPortHole : public PortHole
{
public:
    // Class identification.
    virtual int isA(const char* className) const;

    // Domain-specific initialization.
    PortHole& setPort(const char* name, Block* parent, DataType type = FLOAT);
};

class InXXXPort : public XXXPortHole
{
public:
    // Input/output identification.
    virtual int isItInput() const;

    // Get Particles from input Geodesic.
    virtual void grabData();
};


class OutXXXPort : public XXXPortHole
{
public:
    // Input/output identification.
    virtual int isItOutput() const;

    // Put Particles into the output Geodesic.
    virtual void sendData();
};

 
class MultiXXXPort : public MultiPortHole
{
public:
    // Domain-specific initialization.
    MultiPortHole& setPort(const char* name, Block* parent, DataType type = FLOAT);
};

class MultiInXXXPort : public MultiXXXPort
{
public:
    // Input/output identification.
    virtual int isItInput() const;
 
    // Add a new physical port to the MultiPortHole list.
    virtual PortHole& newPort();
};
 
class MultiOutXXXPort : public MultiXXXPort
{     
public:
    // Input/output identification.
    virtual int isItOutput() const;

    // Add a new physical port to the MultiPortHole list.
    virtual PortHole& newPort();
};

#endif
