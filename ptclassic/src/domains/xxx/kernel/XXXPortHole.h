/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	5 January 1992

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
};

class InXXXPort : public XXXPortHole
{
public:
    // Input/output identification.
    virtual int isItInput() const;
};


class OutXXXPort : public XXXPortHole
{
public:
    // Input/output identification.
    virtual int isItOutput() const;
};

 
class MultiXXXPort : public MultiPortHole
{
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
