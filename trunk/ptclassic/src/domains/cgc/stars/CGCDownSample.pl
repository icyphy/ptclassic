defstar {
	name { DownSample }
	domain { CGC }
	desc { 
A decimator by "factor" (default 2).  The "phase" tells which sample to
output.  If phase = 0, the most recent sample is the output, while if
phase = factor-1 the oldest sample is the output.  Phase = 0 is the
default.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {float}
	}
	state {
		name {factor}
		type {int}
		default {2}
		desc { Downsample factor. }
		attributes { A_SETTABLE }
	}
	state {
		name {phase}
		type {int}
		default {0}
		desc { Downsample phase. }
		attributes { A_SETTABLE }
	}
	constructor {
		noInternalState();
	}
	setup {
		input.setSDFParams(int(factor),int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, ": phase must be < factor");
	}
	codeblock (sendsample) {
	$ref(output) = $ref2(input,phase);
	}
	go {
		addCode(sendsample);
	}
	exectime {
		return 1;
	}
}
