#ifndef _XXXPortHole_h
#define _XXXPortHole_h

/*  Version $Id$

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

    Programmer:		T.M. Parks
    Date of creation:	5 January 1992

    Definitions of domain-specific PortHole classes.
*/

#ifdef __GNUG__
#pragma interface
#endif

#include "PortHole.h"

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
