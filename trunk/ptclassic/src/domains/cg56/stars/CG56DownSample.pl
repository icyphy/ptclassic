defstar {
	name { DownSample }
	domain { CG56 }
	desc { 
A decimator by "factor" (default 2).  The "phase" tells which sample to
output.  If phase = 0, the most recent sample is the output, while if
phase = factor-1 the oldest sample is the output.  Phase = 0 is the
default.
	}
	version { $Id$ }
	author { J. Pino, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {

	}
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {=anytype}
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
 start {
		input.setSDFParams(int(factor),int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, ": phase must be < factor");
	}
	codeblock (sendsample) {
	move	$ref2(input,phase),x0
	move	x0,$ref(output)
	}
	go {
		gencode(sendsample);
	}
	execTime {
		return 2;
	}
}
