defstar {
	name {DownSample}
	domain {SDF}
	desc { 
A decimator by "factor" (default 2).
The "phase" tells which sample to output.
If phase = 0, the most recent sample is the output,
while if phase = factor-1 the oldest sample is the output.
Phase = 0 is the default.
	}
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
.Ir "decimation"
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
	start {
		input.setSDFParams(int(factor),int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, ": phase must be < factor");
	}
	go {
		output%0 = input%int(phase);
	}
}

