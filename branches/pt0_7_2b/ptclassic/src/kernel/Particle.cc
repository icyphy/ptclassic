#include "Particle.h"
#include <stream.h>
#include "Output.h"

extern Error errorHandler;

IntSample :: operator char* ()
	{ return form("%d",data);}

Particle* Plasma :: get()
{
	Particle* p;

	if(size() == 0) {

		// Create a new Particle
		if(type == INT) p = new IntSample(0); 
		
		// Add new Particle types here

		else errorHandler.error(
			"Plasma: Unsupported Particle type requested"
			);
        } else {
                p = Stack::popTop();
	}

	return p;
}

Plasma* PlasmaList :: getPlasma(dataType t)
{
	Plasma* p;

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
