#ifndef _particle_h
#define _particle_h 1

#include "StringList.h"
#include "DataStruct.h"
// include all of the Complex class except the stream functions
#include "ComplexSubset.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/89
 Revisions:


 Particles are the envelope that carry data between Stars
***************************************************************/

	/////////////////////////////////
	// class Particle
	/////////////////////////////////

// Types of data stored in queues between stars
// ANYTYPE is provided for the benefit of Stars like Fork
// and Printer that operate independently of Particle type
enum dataType { INT, FLOAT, STRING, COMPLEX, ANYTYPE };

// Particle is a virtual base class -- you can't declare a Particle.
class Particle
{
public:
	// Identify what type of Particle this is
	virtual dataType readType() = 0;

	// Cast this Particle to an int, float, Complex
	virtual operator int () = 0;
	virtual operator float () = 0;
	virtual operator Complex () = 0;

	// print Particle
	virtual char* print () = 0;

	// Zero or initialize this Particle
	virtual void initialize() = 0;

	// Load the Particle with data
	virtual void operator << (int) = 0;
	virtual void operator << (float) = 0;
	virtual void operator << (Complex&) = 0;

	// Copy a Particle -- since Stars must be able to
	// assign Particles in type-independent fashion,
	// the compiler can't handle this
	virtual Particle& operator = (const Particle&) = 0;

protected:
	// Before copying Particles, always compare their types
	// Otherwise the user could always copy Particles of
	//  incompatible types between an input and output PortHole
	int compareType(Particle& p) {return (readType()==p.readType());}
};

/***************************************************************
* Types of Particles
*
* Different Particle types carry different types of data
*
* To add a new Particle type:
*
*   1. Add a class below for that Particle type
*   2. Add an element to "enum dataType" at the beginning of
*      this file
*   3. Add a statement in class Plasma::get() in Particle.cc
***************************************************************/

	/////////////////////////////////////
	// class IntSample
	/////////////////////////////////////

class IntSample : public Particle
{
public:
	// Tell this world this Particle contains an int
	dataType readType() {return INT;}

	// Cast to an int, float, and Complex
	operator int () {return data;}
	operator float () {return (float)data;}
	operator Complex () {return Complex(data);}

	char* print();

	// Initialize
	IntSample(int i) {data=i;}
	IntSample() {data=0;}

	// Wash the Particle
	void initialize() {data=0;}

	// Load up with data
	void operator << (int i) {data=i;}
	void operator << (float f) {data=(int)f;}
	void operator << (Complex& c) {data=(int)c.real();}

	// Copy the Particle
	Particle& operator = (const Particle&);

private:
	int data;
};

        /////////////////////////////////////
        // class FloatSample
        /////////////////////////////////////
 
class FloatSample : public Particle
{
public:
        // Tell this world this Particle contains an float
        dataType readType() {return FLOAT;}
 
        // Cast to an int, float, and Complex
        operator int () {return (int)data;}
        operator float () {return data;}
	operator Complex () {return Complex(data);}

	char* print();
 
        // Initialize
        FloatSample(float f) {data=f;}
        FloatSample() {data=0.0;}
 
        // Initialize the Particle
        void initialize() {data=0.0;}
 
        // Load up with data
        void operator << (int i) {data=(float)i;}
        void operator << (float f) {data=f;}
        void operator << (Complex& c) {data=(float)c.real();}

        // Copy the Particle
        Particle& operator = (const Particle&);
 private:
        float data;
};

        /////////////////////////////////////
        // class ComplexSample
        /////////////////////////////////////
 
class ComplexSample : public Particle
{
public:
        // Tell this world this Particle contains an float
        dataType readType() {return COMPLEX;}
 
        // Cast to an int, float, Complex
        operator int () {return (int)data.real();}
        operator float () {return (float)data.real();}
	operator Complex () {return data;}

	char* print();
 
        // Initialize
        ComplexSample(Complex c) {data=c;}
	ComplexSample(float f) {data=f;}
	ComplexSample(int i) {data = (float)i;}
        ComplexSample() {data=0.0;}

        // Initialize the Particle
        void initialize() {data=0.0;}

        // Load up with data
        void operator << (int i) {data=Complex(i);}
        void operator << (float f) {data=Complex(f);}
	void operator << (Complex& c) {data=c;}

        // Copy the Particle
        Particle& operator = (const Particle&);
 private:
        Complex data;
};

/****************************************************************
* Plasma
*
* A Plasma is a collection of currently unused Particles
*
* Particles no longer needed are added to the Plasma; any
* OutPortHoles needed a Particle gets it from the Plasma
*
* There is precisely one instance of Plasma for each
*   Particle type needed -- this common pool makes more
*   efficient use of memory
****************************************************************/

	////////////////////////////////////////
	// class Plasma
	////////////////////////////////////////

// Plasma is a type of Stack
class Plasma : public Stack
{
public:
	// Put a Particle into the Plasma
	void put(Particle* p) {pushTop(p);}

	// Get a Particle from the Plasma, creating a
	// new one if necessary; virtual because the particle
	// created must be of proper type
	Particle* get();

	Plasma(dataType t) {type = t;}

private:
	// The type of Particles created by this Plasma
	dataType type;
	friend class PlasmaList;
};

	//////////////////////////////////////////
	// class PlasmaList
	//////////////////////////////////////////

// A PlasmaList is a list of Plasma, where there is one
//   Plasma on the list for each dataType currently used
class PlasmaList : public SequentialList
{
public:
	// Given a dataType, return a pointer to the
	//   corresponding Plasma for that dataType
	Plasma* getPlasma(dataType);
};

#endif
