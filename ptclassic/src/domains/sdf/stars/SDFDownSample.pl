ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
                        All Rights Reserved.

Programmer:  J. Buck
Date of creation: 6/4/90
Modified to use preprocessor: 10/3/90, by J. Buck

DownSample decimates its input.  It's a port of the Gabriel downsample
star.

************************************************************************/
}
defstar {
	name {DownSample}
	domain {SDF}
	desc { 
		"A decimator by factor (default 2).\n"
		"The phase tells which sample to output.\n"
		"phase = 0 outputs the most recent sample,\n"
		"while phase = factor-1 outputs the oldest sample.\n"
		"Phase = 0 is the default."
	}
	input {
		name{input}
		type{ANYTYPE}
	}
	output {
		name{output}
		type{ANYTYPE}
	}
	defstate {
		name {factor}
		type {int}
		default {2}
		desc { "Downsample factor" }
	}
	defstate {
		name {phase}
		type {int}
		default {0}
		desc { "Downsample phase" }
	}
	constructor {
		input.inheritTypeFrom(output);
	}
	start {
		input.setSDFParams(int(factor),int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, ": phase must be < factor");
	}
	go {
		output%0 = input%int(phase);
	}
}

