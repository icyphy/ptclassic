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
	name {FloatRamp}
	domain {DE}
	desc {
	   "Produces a ramp output when stimulated by an input.\n"
	   "The time stamp of the output is the same as that of the input.\n"
	   "The value of the input is ignored."
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
		desc { "Starting and current state of the ramp" }
	}
	defstate {
		name {step}
		type {float}
		default {"1.0"}
		desc { "Size of ramp increments" }
	}

	go {
	   // if the star is triggered, generate ramp output (ignore input).
	   completionTime = arrivalTime;
	   double t = value;
           output.put(completionTime) << float(t);
           t += double(step);
           value = t;
	}
}
