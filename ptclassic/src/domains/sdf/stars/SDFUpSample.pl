defstar {
	name {UpSample}
	domain {SDF}
	desc { 
Upsample by a factor (default 2), filling with fill (default 0.0).
The "phase" tells where to put the sample in an output block.
The default is to output it first (phase = 0).
The maximum phase is "factor" - 1.
	}
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
		name{input}
		type{ANYTYPE}
	}
	output {
		name{output}
		type{=input}
	}
	defstate {
		name {factor}
		type {int}
		default {2}
		desc { Number of samples produced. }
	}
	defstate {
		name {phase}
		type {int}
		default {0}
		desc { Where to put the input in the output block. }
	}
	defstate {
		name {fill}
		type {float}
		default {0.0}
		desc { Value to fill the output block. }
	}
	setup {
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
		// yes, the previous statement really does work correctly
		// even if the type of the output is resolved to COMPLEX
		// or INT.  The appropriate conversion is performed.
	}
}
