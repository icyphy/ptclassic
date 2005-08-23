defstar {
	name {Offset}
	domain {DE}
	desc {
Produces output fixed offset from input float value
	}
	version {@(#)DEOffset.pl	1.5	01 Oct 1996}
	author { Allen Lao }
	copyright { 
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
	location { DE ATM library }
	htmldoc {
Merely produces a DC output (default zero offset) when stimulated
by an input.  The time stamp of the output is the same as
that of the input.  Output is a constant offset from input.
	}
	input {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {float}
	}
	defstate {
		name {offset}
		type {float}
		default {"0.0"}
		desc { Amount to offset input }
	}

	go {
	   completionTime = arrivalTime;
           double  p = double (input.get());
	   output.put(completionTime) << float (double(offset) + p);
	}
}
