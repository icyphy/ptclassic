#ifndef _DynDFPortHole_h
#define _DynDFPortHole_h 1
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1993 The Regents of the University of California.
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

 Programmer:  J. Buck

DFPortHole is the base class for portholes in the various dataflow
domains other than SDF that may be dynamic.

******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif
#include "DFPortHole.h"

// relations: a superset of those in BDF.
// see below for meanings of codes.

enum DFRelation {
	DF_NONE = -1,
	DF_FALSE = 0,
	DF_TRUE = 1,
	DF_SAME = 2,
	DF_COMPLEMENT = 3,
	DF_MULT = 4,
	DF_CASE = 5 };

class DynDFPortHole : public DFPortHole
{
public:
	DynDFPortHole() : pAssocPort(0), relation(DF_NONE) {}
	~DynDFPortHole();
	PortHole* assocPort() { return pAssocPort;}
	int assocRelation() const { return relation;}
	int isDynamic() const;
	void setRelation(DFRelation rel, DynDFPortHole* assoc);
protected:
	void removeRelation();
private:
	DynDFPortHole* pAssocPort;
	// "relation" specifies the relation of this porthole with the
	// assocPort porthole (if it is non-null).  There are five
	// possibilities for BDF ports:
	// DF_NONE - no relationship
	// DF_TRUE - produces/consumes data only when assocBoolean is true
	// DF_FALSE - produces/consumes data only when assocBoolean is false
	// DF_SAME - signal is logically the same as assocBoolean
	// DF_COMPLEMENT - signal is the logical complemnt of assocBoolean
	// DF_MULT - # token transferred is product of value of integer
	//           control token and "numTokens"
	// DF_CASE - tokens transferred only for specific integer token value
	//           (how determined?)
	DFRelation relation;
};

#endif
