/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 5/29/90, J. Buck

This file contains definitions of BDF-specific PortHole classes.

******************************************************************/
#ifndef _BDFPortHole_h
#define _BDFPortHole_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "SDFPortHole.h"

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
	BDF_NONE = -1,
	BDF_FALSE = 0,
	BDF_TRUE = 1,
	BDF_SAME = 2,
	BDF_COMPLEMENT = 3 };


class BDFPortHole : public DFPortHole
{
public:
	BDFPortHole() 
	: pAssocPort(0), relation(BDF_NONE) {}

	~BDFPortHole();

	PortHole* assocPort() { return pAssocPort;}

	// cast is safe because of restrictions on setting of assocPort
	BDFPortHole* assoc() { return (BDFPortHole*)pAssocPort;}

	BDFRelation relType() const {
		return (BDFRelation) assocRelation();
	}

	int assocRelation() const { return relation;}

	// return true if port is dynamic (transfers variable # of tokens)
	int isDynamic() const;

        // The setPort function is redefined to take one more optional
        // argument, a reference to a BDFSigInfo object giving information
	// about the signal.
        PortHole& setPort(const char* portName,
                          Block* parent,
                          DataType type = FLOAT,
			  unsigned numTokens = 1,
			  BDFPortHole* assocBool = 0,
			  BDFRelation relation = BDF_NONE,
			  int delay = 0);

	// Function to set associations.  BDF_SAME and BDF_COMPLEMENT
	// relations always form a loop.
	void setRelation(BDFRelation rel, BDFPortHole* assocBool = 0);

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

	// table for use of "reversals" function
	static BDFRelation reversals[4];
private:
	// if given, points to an associated boolean signal;
	// tokens are only produced/consumed when that signal is true
	BDFPortHole* pAssocPort;

	// "relation" specifies the relation of this porthole with the
	// assocPort porthole (if it is non-null).  There are five
	// possibilities for BDF ports:
	// BDF_NONE - no relationship
	// BDF_TRUE - produces/consumes data only when assocBoolean is true
	// BDF_FALSE - produces/consumes data only when assocBoolean is false
	// BDF_SAME - signal is logically the same as assocBoolean
	// BDF_COMPLEMENT - signal is the logical complemnt of assocBoolean

	// for the latter two cases data are always moved.

	BDFRelation relation;

	void removeRelation();

};

	///////////////////////////////////////////
	// class InBDFPort
	//////////////////////////////////////////

class InBDFPort : public BDFPortHole
{
public:
	int isItInput () const ; // {return TRUE; }

	// Get Particles from input Geodesic
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

	void increment();

	// Move the current Particle in the input buffer -- this
	// method is invoked by the BDFScheduler before go()
	void receiveData();

	// Put the Particles that were generated into the
	// output Geodesic -- this method is invoked by the
	// BDFScheduler after go()
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
