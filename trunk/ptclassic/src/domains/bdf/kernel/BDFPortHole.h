/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 5/29/90, J. Buck

This file contains definitions of BDF-specific PortHole classes.

******************************************************************/
#ifndef _BDFConnect_h
#define _BDFConnect_h 1
#include "Connect.h"

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

// note: FALSE = 0, TRUE = 1 are declared elsewhere.
const int BDF_SAME = 2;
const int BDF_COMPLEMENT = 3;

class BDFPortHole : public PortHole
{
public:
	// number of tokens produced by the porthole
	unsigned numberTokens;

	// if given, points to an associated boolean signal;
	// tokens are only produced/consumed when that signal is true
	PortHole* assocBoolean;

	// "relation" specifies the relation of this porthole with the
	// assocBoolean porthole (if it is non-null).  There are four
	// possibilities:
	// TRUE	- produces/consumes data only when assocBoolean is true
	// FALSE - produces/consumes data only when assocBoolean is false
	// BDF_SAME - signal is logically the same as assocBoolean
	// BDF_COMPLEMENT - signal is the logical complemnt of assocBoolean
	// for the latter two cases data are always moved.
	int relation;

        // The setPort function is redefined to take one more optional
        // argument, a reference to a BDFSigInfo object giving information
	// about the signal.
        PortHole& setPort(const char* portName,
                          Block* parent,
                          dataType type = FLOAT,
			  unsigned numTokens = 1,
			  PortHole* assocBool = 0,
			  int relation = TRUE,
			  int delay = 0) {
		PortHole::setPort(portName,parent,type);
		return setBDFParams(numTokens,assocBool,relation,delay);
	}

	// Function to alter BDF values.
	PortHole& setBDFParams(unsigned numTokens = 1,
			  PortHole* assocBool = 0,
			  int relation = TRUE,
			  int delay = 0);

	// Function to alter BDF values (alternate form)
	PortHole& setBDFParams(unsigned numTokens,
			  PortHole& assocBool,
			  int relation = TRUE,
			  int delay = 0) {
		setBDFParams(numTokens,&assocBool,relation,delay);
	}

	// Services of PortHole that are often used:
	// setPort(dataType d);
	// Particle& operator % (int);
};

	///////////////////////////////////////////
	// class InBDFPort
	//////////////////////////////////////////

class InBDFPort : public BDFPortHole
{
public:
	int isItInput () const ; // {return TRUE; }

	// Get Particles from input Geodesic
	void grabData();

        // Services of PortHole that are often used: 
        // setPort(dataType d); 
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
	void grabData();

	// Put the Particles that were generated into the
	// output Geodesic -- this method is invoked by the
	// BDFScheduler after go()
	void sendData();

        // Services of PortHole that are often used: 
        // setPort(dataType d); 
        // Particle& operator % (int);
};

        //////////////////////////////////////////
        // class MultiBDFPort
        //////////////////////////////////////////
 
// Synchronous dataflow MultiPortHole
 
class MultiBDFPort : public MultiPortHole {
public:
        // The setPort function is redefined to take additional arguments
        // argument, the number of Particles produced
        MultiPortHole& setPort(const char* portName,
                          Block* parent,
			       dataType type = FLOAT,
			  unsigned numTokens = 1,
			  PortHole* assocBool = 0,
			  int rel = FALSE,
			  int del = 0) {
		MultiPortHole::setPort(portName,parent,type);
		numberTokens = numTokens;
		assocBoolean = assocBool;
		relation = rel;
		delay = del;
		return *this;
	}
	void setBDFParams(unsigned n = 1, PortHole* assoc = 0,
			  int rel = TRUE, int del = 0) {
		numberTokens = n;
		assocBoolean = assoc;
		relation = rel;
		del = 0;
	}
	// alternate version with reference arg.
	void setBDFParams(unsigned n, PortHole& assoc,
			  int rel = TRUE, int del = 0) {
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
	PortHole* assocBoolean;

	// specifies relation to the assocBoolean signal (see BDFPortHole)
	int relation;

	// delay args for portholes
	int delay;
 

};

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

#endif
