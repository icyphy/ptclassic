#ifndef _DERCStar_h
#define _DERCStar_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Particle.h"
#include "DERepeatStar.h"
#include "Resource.h"
#include "DERCScheduler.h"
#include "DERCEventQ.h"
class Resource;

class DERCStar : public DERepeatStar
{
public:
	DERCStar();
	/* virtual */ virtual Block* makeNew() const;
	/* virtual */ virtual const char* className() const;
	/* virtual */ virtual int isA(const char*) const;
	/* virtual */ virtual double getDelay();
	/* virtual */ virtual SequentialList* getEvents ();
        /* virtual */ virtual void emitEvent(DERCEvent*, double ) {};
	Resource* resourcePointer;
	char resource[1024];
	double timeOfArrival;
	int priority;
	SequentialList* emittedEvents;
	FILE * Openoverflow ( char *name );
	void Printoverflow ( char *st );
	FILE * Openfiring ( char *name );
	void Printfiring ( char *st );
	void Closeflow ();
        int needsSharedResource;
        int schedPolicy;
	DERCEventQ* interruptQ;

protected:
	static FILE *fpoverflow;
	static FILE *fpfiring;
};
#endif
