defstar {
	name {Sampler}
	domain {DE}
	desc {
Samples "input" at the times given by events on its "clock" input.
	}
	version { $Id$}
	author { E. A. Lee and Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	explanation {
This star samples its data input at the times given by events on
its \fIclock\fR input.
The data value of the \fIclock\fR input is ignored.
If no input is available at the time of sampling, the latest input
is used.  If there has been no input, then a "zero" particle is produced.
The exact meaning of this depends on the particle type.
	}
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
		type {anytype}
	}
	constructor {
		input.inheritTypeFrom(output);
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
