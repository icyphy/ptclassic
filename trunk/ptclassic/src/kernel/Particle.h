#ifndef _particle_h
#define _particle_h 1

#include "DataStruct.h"

// $Id$

/**************************************************************
* Particles
*
* Particles are the envelope that carry data between Stars
*
* Programmer: D.G.Messerschmitt
* Date: Jan. 15, 1990
***************************************************************/

	/////////////////////////////////
	// class Particle
	/////////////////////////////////

// Types of data stored in queues between stars
enum dataType { INT, FLOAT, STRING };

class Particle
{
public:
	// Identify what type of Particle this is
	virtual dataType readType() {};

	// Cast this Particle to an int, float, and char*
	virtual operator int () {return 0;};
	virtual operator float () {return 0.;};
	virtual operator char* () {return "";};

	// Zero or initialize this Particle
	virtual void wash() {};

	// Load the Particle with data
	virtual operator << (int) {};
	virtual operator << (float) {};

	// Clone this Particle -- any past Particles held
	//  in the OutPortHole are clones of the real Particle
	//  put into the Geodesic, to avoid subtle interactions
	//  with the Block receiving the Particles
	// Argument is Particle to be loaded up with my data;
	//  clone() does not create the Particle since that is
	//  the job of Plasma
	// NOTE: it may be possible to avoid this method
	//  by just equating two particles -- try this later
	virtual void clone(Particle*) {};
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

	// Cast to an int, float, and char*
	operator int () {return data;}
	operator float () {return (float)data;}
	operator char* ();

	// Initialize
	IntSample(int i) {data=i;}
	IntSample() {data=0;}

	// Wash the Particle
	void wash() {data=0;}

	// Load up with data
	operator << (int i) {data=i;}
	operator << (float f) {data=(int)f;}

	// Clone the Particle
	void clone(Particle* p) {((IntSample*)p)->data = data;}

private:
	int data;
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
