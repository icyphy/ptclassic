#ifndef _ACSPortHole_h
#define _ACSPortHole_h

/*  Version @(#)ACSPortHole.h	1.13 09/08/99

@Copyright (c) 1998 The Regents of the University of California.
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

    Programmer:		T.M. Parks
    Date of creation:	5 January 1992

    Definitions of domain-specific PortHole classes.
*/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGPortHole.h"
#include "ACSGeodesic.h"
#include "PrecisionState.h"

// JMS
// strstream.h is not part of g++-2.95.1
//#include <strstream.h>

extern const bitWord PB_GLOBAL /* = 0x20 */;
extern const Attribute P_GLOBAL;
extern const Attribute P_LOCAL;

// three different types of buffer.
// If owner, this porthole owns buffer.
// If embedded, no physical buffer is assigned to that porthole.
enum BufType { NA, OWNER, EMBEDDED };

// a holder for precisions of fixed-point variables;
// this class is used in conjunction with the % operators of class ACSPortHole
// in order to achieve a consistent method to refer to port precisions (s.below)
class ACSPrecisionHolder : private Precision {
public:
	ACSPrecisionHolder(int len, int intb,
            const char* symbolic_length=NULL, const char* symbolic_intBits=NULL) :
		Precision(len,intb, symbolic_length,symbolic_intBits) { }
	ACSPrecisionHolder(const Precision& p) : Precision(p) { }

	Precision precision() { return *this; }
};


class ACSPortHole : public CGPortHole
{
friend class ACSForkDestIter;
public:

        // Initializations moved to ACSPortHole.cc
	ACSPortHole() : CGPortHole(),
	     // JMS 
	     port_BW(0.0), port_id(-1), star_id(-1),
	     path_id(-1), assigned(FALSE),
	     maxBuf(1), manualFlag(0),
	     hasStaticBuf(1), asLinearBuf(1), myType(NA),
	     converted(0), bufName(0), manualOffset(0) {}
	~ACSPortHole();

    // Class identification.
    //virtual int isA(const char* className) const;

	void initialize();

	ACSPortHole* getForkSrc() { return (ACSPortHole*) forkSrc; }

	// const version
	const ACSPortHole* getForkSrc() const {
		return (const ACSPortHole*) forkSrc;
	}

	//int localBufSize() const;

	// return the far port bypassing the fork stars
	ACSPortHole* realFarPort();
	// const version
	const ACSPortHole* realFarPort() const;

	// make a complete list of forkDests after resolving the indirect
	// list via fork stars
	void setupForkDests();

	// return the buffer requirements. Indicates whether static buffering
	// is achieved or not.
	// return numTokens if on the wormhole boundary
	int maxBufReq() const;

	// return TRUE if a star can see this port as a linear buffer.
	int linearBuf() const { return asLinearBuf; }

	// return TRUE if the actual static buffering achieved.
	int staticBuf() const { return hasStaticBuf; }
	// give up static buffering for loop scheduling
	void giveUpStatic() { hasStaticBuf = FALSE; }

	// set and get the buffer type
	void setBufferType();

	BufType  bufType() const;

	// name the porthole in the data structure.
	void setGeoName(const char* n);
	const char* getGeoName() const;

	// Return the geodesic connected to this PortHole.
	// This is typesafe because allocateGeodesic
	// makes myGeodesic of this type.
	ACSGeodesic& geo() { return *(ACSGeodesic*)myGeodesic;}

	// const version
	const ACSGeodesic& geo() const {
		return *(const ACSGeodesic*)myGeodesic;}

	// buf Size
	int bufSize() const { return maxBufReq(); }

	// return bufferSize
	int inBufSize() const { return bufferSize; }

	// initialize offset
	int initOffset();

	// determine the buffer size for this porthole finally
	// Consider delay and need of past particles.
	void finalBufSize(int statBuf);

	// Set the maxBuf manually. In finalBufSize() method, we will
	// compare this manual value with what the current scheduler
	// calculates. If this manual value is smaller, signal an error.
	void requestBufSize(int i) { maxBuf = i; manualFlag = TRUE; }

	// determine the property of the buffer associated with this port.
	void setFlags();

	// set output offset manually
	void setOffset(int v) { manualOffset = v; }

 	// set the precision for a port of type FIX;
 	// the symbolic precision representation is ignored for ports with
 	// attribute A_VARPREC since their symbolic representation comprises
 	// references to precision variables declared in the generated code;
 	// this scheme is fixed and cannot be changed at compile time.
 	void setPrecision(const Precision& p) { prec = p; }
 
 	// return the precision for a port of type FIX;
 	// for ports with attribute A_VARPREC the symbolic representation
 	// of the precision may depend on the offset into the porthole
 	// buffer;  therefore we adapt the scheme used in the SDF domain
 	// to specify a buffer offset directly with the % operator.
 	// The precision of a specific buffer entry can be obtained by
 	// using expressions like "(output%1).precision()".
 
 	// overloaded % operator for constant and symbolic integer offsets
 	ACSPrecisionHolder operator ^ (int offset) const;
 	ACSPrecisionHolder operator ^ (const char* symbolic_offset) const;
 
 	// abbreviation for 0 offsets
 	Precision precision() const { return (*this^"0").precision(); }

 	// return TRUE if the precision assigned to this port is valid
	int validPrecision() const { return prec.isValid(); }

        // JMS
        float port_BW;   // Bandwidth of connection(s) in bits
        int port_id;     // Unique identifier for the port
        int star_id;     // Acs id of the star this port is connected to
        int remote_port_id; // Unique identifier of the destination port
        int path_id;     // A unique identifier for the link between stars
        int assigned;    // Boolean, true if a name has been bound to this port


protected:

    // Allows Target to change Plasma type for fixed-point simulation.
    /* virtual */ int allocatePlasma();

 	// return the precision of this or the connected port that was
 	// assigned with the setPrecision() method
 	Precision getAssignedPrecision() const;
 

private:

	int maxBuf;		// Final buffer size.
	short manualFlag;	// set if buffer size is manually chosen.
	short hasStaticBuf;	// set if static buffer is achieved.
	short asLinearBuf;	// set if acts as a linear buf inside a star
	BufType myType;		// buffer Type.
	
	int converted;		// set TRUE if the explicit type conversion 
				// is required.

	char* bufName;		// set if no geodesic is assigned.

	SequentialList& myDest() { return forkDests; }
	
	int manualOffset;	// set the offset of the output manually.

	Precision prec;		// fixed point precision for ports of type FIX

};

class InACSPort : public ACSPortHole
{
public:

    // Input/output identification.
    int isItInput() const;

	// Particle I/O for simulation.
	virtual void receiveData();

};


class OutACSPort : public ACSPortHole
{
public:
    // Input/output identification.
    int isItOutput() const;

	// Particle I/O for simulation.
	virtual void receiveData();
	virtual void sendData();
};

 
class MultiACSPort : public MultiCGPort
{
public:
	int someFunc();

	// return the precision for a multiport of type FIX;
	// if the precision has not been set explicitly via the setPrecision()
  	// method, a combination of the precisions of the connected portholes
	// is returned;
	// note that the attribute A_VARPREC is left unregarded since precision
	// variables are not defined for multiports.  In order to refer to the
	// fix_prec variable of a multiport one must refer to the derived normal
	// portholes.
  	Precision precision() const;
  
	// set the fixed-point precision
	void setPrecision(const Precision& p) { prec = p; }

	// return TRUE if the precision is set for this port
	int validPrecision() const { return prec.isValid(); }

private:
	Precision prec;		// fixed point precision for ports of type FIX
};

class MultiInACSPort : public MultiACSPort
{
public:
    // Input/output identification.
    int isItInput() const;
 
    // Add a new physical port to the MultiPortHole list.
    PortHole& newPort();
};
 
class MultiOutACSPort : public MultiACSPort
{     
public:
    // Input/output identification.
    int isItOutput() const;

    // Add a new physical port to the MultiPortHole list.
    PortHole& newPort();
};

class ACSForkDestIter : private ListIter {
public:
	ACSForkDestIter(ACSPortHole* p) : ListIter(p->myDest()) {}
	ACSPortHole* next() { return (ACSPortHole*) ListIter::next(); }
	ACSPortHole* operator++ (POSTFIX_OP) { return next(); }
	ListIter::reset;
};


#endif
