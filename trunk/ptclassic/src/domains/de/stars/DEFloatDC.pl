defstar {
	name {FloatDC}
	domain {DE}
	version { $Id$}
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
Produces a DC output (default value is zero) when stimulated
by an input.  The time stamp of the output is the same as
that of the input. The value of the input is ignored.
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
		desc { Value of the output particles. }
	}

	go {
	   // if the star is triggered, generate DC output (ignore input).
	   completionTime = arrivalTime;
	   output.put(completionTime) << double(value);
	}
}
