defstar {
	name {Ramp}
	domain {DE}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
Produce an output event with a monotic value when stimulated
by an input event. The value of the output event starts at "value" and
increases by "step" each time the star fires. The value of the input is ignored.
	}
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {float}
	}
	defstate {
		name {value}
		type {float}
		default {"0.0"}
		desc { Starting and current state of the ramp. }
	}
	defstate {
		name {step}
		type {float}
		default {"1.0"}
		desc { Size of the ramp increments. }
	}

	go {
	   // if the star is triggered, generate ramp output (ignore input).
	   completionTime = arrivalTime;
	   double t = value;
           output.put(completionTime) << t;
           t += double(step);
           value = t;
	}
}
