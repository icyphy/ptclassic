defstar {
	name { PulseGen }
	derivedFrom {RepeatStar}
	domain {DE}
	desc {
This star generates events with specified values at specified moments.
The events are specified in the "value" array, which consists of
time-value pairs, given in the syntax of complex numbers.  This star
assumes that the time-value pairs appear in chronological order
(ascending with respect to time) and that no simultaneous events
occur (all time values are unique).
	}
	version { 1.6 06/04/96 }
	author { Stefan De Troch (of IMEC) }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	output {
		name { output }
		type { float }
	}
	defstate {
		name { value }
		type { complexarray }
		default { "(0,1) (1,0)" }
	}
	protected {
		int pos;
	}
	setup {
		pos = 0;
	}
	go {
		// first event is scheduled at time 0 by RepeatStar
		// check whether the first event is too early
		if (arrivalTime >= real(value[pos])) {
		    output.put(real(value[pos])) << double(imag(value[pos]));
		    pos++;
		}
		if (pos < value.size()) {
		    refireAtTime(real(value[pos]));
		}
	}
}
