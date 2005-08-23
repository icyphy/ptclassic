static const char file_id[] = "MutableCQEventQueue.cc";
/**************************************************************************
Yadi Yadi Yada
******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MutableCQEventQueue.h"
// #include "Particle.h"

Event* MutableCQEventQueue:: getEvent(Particle* p, PortHole* ph) {
	Event* temp;
	// a free event instance
	if (freeEventHead) {
		temp = freeEventHead;
		freeEventHead  = temp->next;
	} else {
		LOG_NEW; temp = new Event;
	}
	// set the event members.
	temp->dest = ph;
	temp->p = p;
	return temp;
}

// delete the free events.
void MutableCQEventQueue:: clearFreeEvents() {
	if (!freeEventHead) return;
	while (freeEventHead->next) {
		Event* temp = freeEventHead;
		freeEventHead = freeEventHead->next;
		LOG_DEL; delete temp; 
	}
	LOG_DEL; delete freeEventHead;
	freeEventHead = 0;
}

// Put the unused particles into the plasmas.
void MutableCQEventQueue:: clearParticles() {
	Event* temp = freeEventHead;
	while (temp) {
		if (temp->p) temp->p->die();
		temp = temp->next;
	}
}

void MutableCQEventQueue:: putFreeLink(CqLevelLink* p) {
	if (p->fineLevel) {
		Event* temp = (Event*) p->e;
		putEvent(temp);
	}
	MutableCalendarQueue:: putFreeLink(p);
}

CqLevelLink * MutableCQEventQueue::createRefInDestinedEventList( CqLevelLink * link )
{
	// Create reference in DestinedEventList and point CqLevelLink
	// this reference. 
	// FIXME: Will there be problems with the semantics of this
	// statement; i.e., circular dependencies??
	DEStar *destinationStar = (DEStar*) (link->dest);
	link->destinedLink = destinationStar->addRefToDestinedEventList( link ); 
	return link;
}

void MutableCQEventQueue :: initialize() {
	// first maintain free links and free events.
	clearFreeEvents();
	MutableCalendarQueue :: initialize();
	clearParticles();
}

MutableCQEventQueue :: ~MutableCQEventQueue() {
	initialize();
	clearFreeEvents();
}
