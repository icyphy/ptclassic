/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/89
 Revisions:

 Definition of some methods in Particle.h

**************************************************************************/

#include "Particle.h"
#include <stream.h>
#include "Output.h"

extern Error errorHandler;

	///////////////////////////////////////
	// class IntSample
	///////////////////////////////////////

Particle& IntSample :: operator = (const Particle& p)
{
	if(compareType(p)) {
		// Types are compatible, so we can copy
		data = int(p);
		}
	else
		errorHandler.error(
		"Particle: attempt to assign incompatible Particle types"
			);
	return *this;
}

char* IntSample :: print () const { return form("%d",data);}


	////////////////////////////////////////
	// class FloatSample
	////////////////////////////////////////

Particle& FloatSample :: operator = (const Particle& p)
{
        if(compareType(p)) {
                // Types are compatible, so we can copy
                data = float(p);
                }
        else
                errorHandler.error(
                "Particle: attempt to assign incompatible Particle types"
                        );
	return *this;
}

char* FloatSample :: print () const { return form("%f",data);}

        ////////////////////////////////////////
        // class ComplexSample
        ////////////////////////////////////////

Particle& ComplexSample :: operator = (const Particle& p)
{
        if(compareType(p)) {
                // Types are compatible, so we can copy
                data = Complex(p);
                }
        else
                errorHandler.error(
                "Particle: attempt to assign incompatible Particle types"
                        );
        return *this;
}

char* ComplexSample :: print () const
        { return form("(%f,%f)",data.real(),data.imag());}


	////////////////////////////////////////
	// class Plasma
	////////////////////////////////////////

Particle* Plasma :: get()
{
	Particle* p;

	if(size() == 0) // A new Particle has to be created

		switch (type) {

			case INT:
				p = new IntSample(0); 
				break;

			case FLOAT:
				p = new FloatSample(0); 
				break;

			case COMPLEX:
				p = new ComplexSample(0);
				break;
		
			/**********************************************
			***********************************************
			* NOTE: Add new Particle types here ***********
			***********************************************
			**********************************************/

			default:
				errorHandler.error(
				"Plasma: Unsupported Particle type requested"
				);
				p = NULL;
			}
        else {
		// need the cast to avoid a warning
                p = (Particle *)Stack::popTop();
		p->initialize();
	}

	return p;
}

	/////////////////////////////////////////
	// class PlasmaList
	/////////////////////////////////////////

Plasma* PlasmaList :: getPlasma(dataType t)
{
	Plasma* p;

	if(t == ANYTYPE)
		errorHandler.error(
		"PlasmaList: cannot create Plasma with dataType=ANYTYPE"
				);

	// Find a Plasma, if one with that type is already on the list
	for(int i= size(); i>0; i--) {
		p = (Plasma*)next();
		if(p->type == t )
			return p;
		}

	// One doesnt exist, so we have to create it
	p = new Plasma(t);
	// Add it to the list
	put(p); 	

	return p;
}
