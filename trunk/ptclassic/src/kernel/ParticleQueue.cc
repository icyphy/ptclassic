static const char file_id[] = "ParticleQueue.cc";
#ifdef __GNUG__
#pragma implementation
#endif
/***************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

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
