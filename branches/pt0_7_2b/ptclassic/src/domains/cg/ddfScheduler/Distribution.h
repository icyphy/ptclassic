/**************************************************************************
Version identification:
$Id$

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

 Programmer:  Soonhoi Ha
 Date of creation: 7/2/91

 Definition of some statistical distributions.

**************************************************************************/
#ifndef _Distribution_h
#define _Distribution_h 1
#ifdef __GNUG__
#pragma interface
#endif

class MultiTarget;
class StringList;
class Galaxy;

class DistBase {
public:
	// virtual functions.
	virtual StringList printParams(); 
	virtual int setParams(Galaxy*, MultiTarget*);	// read the parameters.
	virtual DistBase* copy();		// copy the right distribution.
	virtual void paramInfo();		// information on parameters.
	virtual double assumedValue();		// return the assumed value.
	virtual const char* myType() const;

	int setType(Galaxy*, MultiTarget*);		// set the type.
	int readType() 	{ return type; }
	DistBase* myParam()	{ return params; }
	DistBase() { type = -1;  params = (DistBase*) 0; }
	virtual ~DistBase();

private:
	int type;		// type of distribution.
	DistBase* params;	// actual distribution.
};

#endif
