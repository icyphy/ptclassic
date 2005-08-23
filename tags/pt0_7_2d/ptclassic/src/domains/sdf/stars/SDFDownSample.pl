defstar {
	name {DownSample}
	domain {SDF}
	desc { 
A decimator by a given "factor" (default 2).
The "phase" tells which sample to output.
If phase = 0, the most recent sample is the output,
while if phase = factor-1 the oldest sample is the output.
Phase = 0 is the default.  Note that "phase" has the opposite
sense of the phase parameter in the UpSample star, but the
same sense as the phase parameter in the FIR star.
	}
	version {@(#)SDFDownSample.pl	2.12	10/01/96}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
<a name="decimation"></a>
	}
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
		desc { Downsample factor. }
	}
	defstate {
		name {phase}
		type {int}
		default {0}
		desc { Downsample phase. }
	}
	setup {
		input.setSDFParams(int(factor),int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, ": phase must be < factor");
	}
	go {
		output%0 = input%int(phase);
	}
}

