#ifndef _EventQueue_h
#define _EventQueue_h 1
#ifdef __GNUG__
#pragma interface
#endif

// EWK
#include "CalendarQueue.h"

class Particle;
class PortHole;

        //////////////////////////////////////////
        // class Event and EventQueue
        //////////////////////////////////////////

class Event
{
public:	
	PortHole* dest;		// destination PortHole
	Particle* p;		// particle or event
	Event*	  next;		// form a event-list

	Event() : next(0) {}
};

// EWK
class EventQueue : public CalendarQueue
{
public:
	void pushHead(Particle* p, PortHole* ph, double v, double fv) {
            Event* temp = getEvent(p, ph);
	    Star *dest;
	    PortHole* tl = 0;
	    if (fv != 0) {
		tl = temp->dest;
		dest = &tl->parent()->asStar();
	    } else {
		dest = (Star*) temp;
	    }
            levelput(temp, v, fv, dest);
	}
	void pushTail(Particle* p, PortHole* ph, double v, double fv) {
	    pushHead(p, ph, v, fv);
	}

	void putFreeLink(CqLevelLink* p); // virtual

	// put the residual events and particles into the free stores.
	void initialize();

	EventQueue() : freeEventHead(0) {}
	~EventQueue();
private:
	Event*  freeEventHead;
	Event*  getEvent(Particle*, PortHole*);
	void putEvent(Event* e) {
	        e->next = freeEventHead;
	        freeEventHead = e;
             }
	void clearFreeEvents();
	void clearParticles();
};

#endif
