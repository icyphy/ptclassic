ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
                        All Rights Reserved.

Programmer:  J. Buck
Date of creation: 6/4/90
Modified to use preprocessor: 10/3/90, by J. Buck

UpSample interpolates its input.  It is a port of the Gabriel upsample
star.

************************************************************************/
}
defstar {
	name {UpSample}
	domain {SDF}
	desc { 
		"Upsample by a factor (2), filling with fill (0.0).\n"
		"The phase tells where to put the sample in an output block.\n"
		"The default is to output it first (phase = 0).\n"
		"The maximum phase is factor - 1."
	}
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}
	defstate {
		name {factor}
		type {int}
		default {2}
		desc { "Upsample factor" }
	}
	defstate {
		name {phase}
		type {int}
		default {0}
		desc { "Upsample phase" }
	}
	defstate {
		name {fill}
		type {float}
		default {0.0}
		desc { "fill value" }
	}
	start {
		output.setSDFParams(int(factor),int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, ": phase must be < factor");
	}
	go {
		int i, match = int(factor)-int(phase)-1;
		for (i = 0; i < int(factor); i++)
			if (i == match)
				output%i = input%0;
			else
				output%i << double(fill);
	}
}
