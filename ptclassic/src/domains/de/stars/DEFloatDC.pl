ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  S. Ha
 Date of creation: 8/31/90
 Converted to use preprocessor, 9/29/90, by E. A. Lee

 This star produces a DC output (default value is zero) when stimulated
 by an input.  The time stamp of the output is the same as that of the
 input.

**************************************************************************/
}
defstar {
	name {FloatDC}
	domain {DE}
	desc {
		"Produces a DC output (default value is zero) when stimulated\n"
 		"by an input.  The time stamp of the output is the same as\n"
		"that of the input. The value of the input is ignored."
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
		desc { "Value of output events" }
	}

	go {
	   // if the star is triggered, generate DC output (ignore input).
	   completionTime = arrivalTime;
	   output.put(completionTime) << float(double(value));
	}
}
