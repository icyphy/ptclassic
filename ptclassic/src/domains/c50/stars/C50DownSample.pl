defstar {
	name { DownSample }
	domain { C50 }
	desc { 
A decimator by "factor" (default 2).  The "phase" tells which sample to
output.  If phase = 0, the most recent sample is the output, while if
phase = factor-1 the oldest sample is the output.  Phase = 0 is the
default.
	}
	version { $Id$ }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 control library }
	input {
		name {input}
		type {ANYTYPE}
	}
	output {
		name {output}
		type {=input}
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
 setup {
		input.setSDFParams(int(factor),int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, ": phase must be < factor");
	}
	codeblock (sendsample) {
	splk	#$addr(input,phase),BMAR	;Address needed input Sample
	bldd	BMAR,#$addr(output)		;Output = needed input sample
	}
	go {
		addCode(sendsample);
	}
	execTime {
		return 3;
	}
}


