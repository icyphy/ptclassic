defstar {
	name {FloatRamp}
	domain {DE}
	version { $Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	desc {
This star produces a ramp output when stimulated by an input.
A ramp is simply a monotonically increasing signal.
The time stamp of the output is the same as that of the input.
The value of the input is ignored.
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
