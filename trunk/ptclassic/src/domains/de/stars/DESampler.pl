defstar {
	name {Sampler}
	domain {DE}
	desc {
Sample the input at the times given by events on the "clock" input.
The data value of the "clock" input is ignored.
If no input is available at the time of sampling, the latest input
is used.  If there has been no input, then a "zero" particle is produced.
The exact meaning of this depends on the particle type.
	}
	version { $Id$}
	author { E. A. Lee and Soonhoi Ha }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	input {
		name {input}
		type {anytype}
	}
	input {
		name {clock}
		type {anytype}
	}
	output {
		name {output}
		type {=input}
	}
	constructor {
		input.triggers();
		input.before(clock);
	}
	go {
	   // Check to see whether the star was triggered by a clock input
	   if (clock.dataNew) {
		completionTime = arrivalTime;
		output.put(completionTime) = input%0;
		clock.dataNew = FALSE;
	   }
	}
}
