defstar {
	name { Match }
	domain { DE }
	version { $Id$ }
	author { Soonhoi Ha and E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	desc {
NOT WORKING YET.  NEED A WAY TO COMPARE PARTICLES.
Given a particle on the "match" input, look for a previously received
particle from the "reference" input with the same value.  If a match occurs,
the matching value is sent to the "matchOut" output, and the time difference
between them is sent to the "timeDiff" output.
	}
	explanation {
Particles arriving on the "reference" input are stored.  When a particle
arrives on the "match" input, it is checked against all particles that have
previously arrived on the "reference" input.
To check for a match, the print method of the particle is used to generate
a string for each particle, and the strings are compared.
This method works for all types of particles.
.pp
When a match occurs, the "reference" particle is removed from internal storage.
When no match occurs, no outputs are produced, and the "match" particle is
discarded.  This star can be used, for example, to measure sojourn times
in queueing systems.
	}
	seealso {MeasureDelay}
	input {
		name { reference }
		type { ANYTYPE }
	}
	input {
		name { match }
		type { ANYTYPE }
	}
	output {
		name { timeDiff }
		type { float }
	}
	output {
		name { matchOut }
		type { =reference }
	}
	defstate {
		name {capacity}
		type {int}
		default {"-1"}
		desc {
Maximum size of internal store. If <0, capacity is infinite.
		}
	}
	hinclude { "PriorityQueue.h" }
	protected {
		// Use a priority queue with the time stamp used as the
		// priority.  This is simply a convenient way to store
		// both the value and its arrival time in a pre-existing
		// data structure.
		PriorityQueue refQ;
	}
	constructor {
		// Indicate that an event on the "match" input triggers
		// an immediate output.
		match.triggers(timeDiff);

		// Hint to the scheduler that if there is any chance of
		// simultaneous events on the two inputs, then the "reference"
		// input must be available by the time the "matca" input is
		// processed.
		reference.before(match);
	}
	start {
		refQ.initialize();
	}

	go {
		// get input
		completionTime = arrivalTime;
		// Process all pending "reference" inputs first.
		while(reference.dataNew) {
		    // Store data on the queue
		    Particle& refval = reference%0;
		    Particle* newp = refval.clone();

		    // reference inputs are put at the tail of the queue
		    // Check for capacity first
		    refQ.levelput(newp, arrivalTime);
		    reference.getSimulEvent();
		}
		while(match.dataNew) {
		    Particle& matchval = match%0;

		    // go though the first queue to check if the
		    // data value is there
		    int len = refQ.length();

		    refQ.reset();
		    while (len > 0) {
			LevelLink* h = refQ.next();
			Particle* oldp = (Particle*) h->e;
			if (matchval.print() == oldp->print()) {
			// if (!strcmp(matchval.print(), oldp->print()))
			   // yes, match! --> output
			   timeDiff.put(completionTime) <<
				   completionTime - h->level;
			   matchOut.put(completionTime) = *oldp;
			   oldp->die();
			   refQ.extract(h);
			   break;
			}
			len--;
		    }
		    match.getSimulEvent();
		}
	}
}
