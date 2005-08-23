defstar {
	name {UpSample}
	domain {SDF}
	desc { 
Upsample by a given "factor" (default 2), giving inserted samples the
value "fill" (default 0.0).  The "phase" parameter (default 0) tells where
to put the sample in an output block.  A "phase" of 0 says to output
the input sample first followed by the inserted samples. The maximum
"phase" is "factor" - 1.  Although the "fill" parameter is a floating-point
number, if the input is of some other type, such as complex, then fill
particle will be obtained by casting "fill" to the appropriate type.
	}
	version {@(#)SDFUpSample.pl	2.10 04/27/96}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
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
