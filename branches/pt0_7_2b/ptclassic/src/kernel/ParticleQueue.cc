static const char file_id[] = "ParticleQueue.cc";
#ifdef __GNUG__
#pragma implementation
#endif
/***************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 5/22/91
 Revisions:

A ParticleQueue maintains a queue of particles with a possibly finite
capacity.  It is built around the ParticleStack class.

Operations: getq inserts a particle into the queue, putq gets a
particle from the queue and copies its data into the user-supplied
particle.  setCapacity sets the capacity.

getq fails (returning FALSE) if there is no data in the queue;
putq fails (returning FALSE) if the queue is full.

empty and full are predicates returning true if the queue is empty/full.
length returns the queue length, capacity returns the capacity.
initialize discards the particles in the queue and optionally sets
the capacity.

***************************************************************/
#include "ParticleQueue.h"

// put a particle into the queue: fails if capacity exceeded
int ParticleQueue::putq(Particle& p) {
	if (full()) return FALSE;
	else {
		stk.putTail(p.clone());
		count++;
		return TRUE;
	}
}

// get a particle from the queue, copy into user-supplied particle
// fails if queue empty

int ParticleQueue::getq(Particle& p) {
	if (empty()) return FALSE;
	Particle* qp = stk.get();
	count--;
	p = *qp;
	qp->die();
	return TRUE;
}

// modify the capacity
void ParticleQueue::setCapacity(int sz) {
	if (sz < 0) maxCount = QSIZE_UNLIM;
	else maxCount = sz;
}
