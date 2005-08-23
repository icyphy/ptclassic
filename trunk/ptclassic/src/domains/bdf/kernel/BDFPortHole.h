/**************************************************************************
Version identification:
@(#)BDFPortHole.h	2.13	3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
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

 Programmer:  J. Buck
 Date of creation: 5/29/90, J. Buck

This file contains definitions of BDF-specific PortHole classes.

******************************************************************/
#ifndef _BDFPortHole_h
#define _BDFPortHole_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "DynDFPortHole.h"

/*****************************************************************
BDF: Synchronous Data Flow generalized to handle booleans

This is a generalization of SDF to include booleans.  In SDF,
each PortHole promises to consume or generate a fixed
number of Particles each time the Star is invoked.  In BDF,
each PortHole will consume or produce one of two values, based
on the value of an optional "associated Boolean" porthole.
Ordinary SDF stars may also be run in the BDF domain, but
constructs such as Switch and Select are also permitted.

****************************************************************/


        //////////////////////////////////////////
        // class BDFPortHole
        //////////////////////////////////////////

// Contains all the special features required for
//   synchronous dataflow (BDF)

// see below for meanings of relation codes.

enum BDFRelation {
	BDF_NONE = DF_NONE,
	BDF_FALSE = DF_FALSE,
	BDF_TRUE = DF_TRUE,
	BDF_SAME = DF_SAME,
	BDF_COMPLEMENT = DF_COMPLEMENT };


class BDFPortHole : public DynDFPortHole
{
public:
	BDFPortHole() : alreadyReadFlag(0) {}

	// cast is safe because of restrictions on setting of assocPort
	BDFPortHole* assoc() { return (BDFPortHole*)assocPort();}

	BDFRelation relType() const {
		return (BDFRelation) assocRelation();
	}

	// return true if port is dynamic (transfers variable # of tokens)
	int isDynamic() const;

	// Function to set associations.  BDF_SAME and BDF_COMPLEMENT
	// relations always form a loop.
	void setRelation(BDFRelation rel, BDFPortHole* assocBool = 0) {
		DynDFPortHole::setRelation(DFRelation(rel),assocBool);
	}

	// Function to alter BDF values.
	PortHole& setBDFParams(unsigned numTokens = 1,
			  BDFPortHole* assocBool = 0,
			  BDFRelation relation = BDF_NONE,
			  int delay = 0);

	// Function to alter BDF values (alternate form)
	PortHole& setBDFParams(unsigned numTokens,
			  BDFPortHole& assocBool,
			  BDFRelation relation = BDF_NONE,
			  int delay = 0) {
		return setBDFParams(numTokens,&assocBool,relation,delay);
	}

	// fns for the "already read" flag, for dynamic execution.
	// flag is set by receiveData in input portholes.
	int alreadyRead() const { return alreadyReadFlag;}
	void clearReadFlag() { alreadyReadFlag = FALSE;}

	// table for use of "reversals" function
	static BDFRelation reversals[4];
protected:
	int alreadyReadFlag;
};

	///////////////////////////////////////////
	// class InBDFPort
	//////////////////////////////////////////

class InBDFPort : public BDFPortHole
{
public:
	int isItInput () const ; // {return TRUE; }

	void receiveData();

        // Services of PortHole that are often used: 
        // setPort(DataType d); 
        // Particle& operator % (int);
};

	////////////////////////////////////////////
	// class OutBDFPort
	////////////////////////////////////////////

class OutBDFPort : public BDFPortHole
{
public:
        int isItOutput () const; // {return TRUE; }

	void receiveData();
	void sendData();

        // Services of PortHole that are often used: 
        // setPort(DataType d); 
        // Particle& operator % (int);
};

        //////////////////////////////////////////
        // class MultiBDFPort
        //////////////////////////////////////////
 
// Boolean dataflow MultiPortHole
 
class MultiBDFPort : public MultiPortHole {
public:
        // The setPort function is redefined to take additional arguments
        // argument, the number of Particles produced
        MultiPortHole& setPort(const char* portName,
                          Block* parent,
			       DataType type = FLOAT,
			  unsigned numTokens = 1,
			  BDFPortHole* assocBool = 0,
			  BDFRelation rel = BDF_NONE,
			  int del = 0) {
		MultiPortHole::setPort(portName,parent,type);
		numberTokens = numTokens;
		assocBoolean = assocBool;
		relation = rel;
		delay = del;
		return *this;
	}
	void setBDFParams(unsigned n = 1, BDFPortHole* assoc = 0,
			  BDFRelation rel = BDF_NONE, int del = 0) {
		numberTokens = n;
		assocBoolean = assoc;
		relation = rel;
		del = 0;
	}
	// alternate version with reference arg.
	void setBDFParams(unsigned n, BDFPortHole& assoc,
			  BDFRelation rel = BDF_NONE, int del = 0) {
		numberTokens = n;
		assocBoolean = &assoc;
		relation = rel;
		del = 0;
	}

protected:
	// number of tokens produced by the porthole
	unsigned numberTokens;

	// if given, points to an associated boolean signal;
	// tokens are only produced/consumed when that signal is true
	BDFPortHole* assocBoolean;

	// specifies relation to the assocBoolean signal (see BDFPortHole)
	BDFRelation relation;

	// delay args for portholes
	int delay;
 

};

// test to see if a BDF/SDF port is conditional
inline int conditional(const DFPortHole& p) {
	int r = p.assocRelation();
	return (r == BDF_TRUE || r == BDF_FALSE);
}


        //////////////////////////////////////////
        // class MultiInBDFPort
        //////////////////////////////////////////
        
// MultiInBDFPort is an BDF input MultiPortHole
 
class MultiInBDFPort : public MultiBDFPort {
public:
        int isItInput () const; // {return TRUE; }
 
        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();
};
 
 
        //////////////////////////////////////////
        // class MultiOutBDFPort
        //////////////////////////////////////////

// MultiOutBDFPort is an BDF output MultiPortHole  

class MultiOutBDFPort : public MultiBDFPort {     
public:
        int isItOutput () const; // {return TRUE; }

        // Add a new physical port to the MultiPortHole list
        PortHole& newPort();
};

// common tests on BDFRelations
inline int TorF(BDFRelation r) { return r == BDF_TRUE || r == BDF_FALSE;}
inline int SorC(BDFRelation r) { return r == BDF_SAME || r == BDF_COMPLEMENT;}

inline BDFRelation reverse(BDFRelation r) {
	return (r == BDF_NONE) ? r : BDFPortHole::reversals[r];
}

#endif
