defstar {
	name { DownSample }
	domain { CG56 }
	desc { 
A decimator by a given "factor" (default 2).
The "phase" tells which sample to output.
If phase = 0, the most recent sample is the output,
while if phase = factor-1 the oldest sample is the output.
Phase = 0 is the default.  Note that "phase" has the opposite
sense of the phase parameter in the UpSample star, but the
same sense as the phase parameter in the FIR star.
	}
	htmldoc {
<a name="decimation"></a>
	}
	version { $Id$ }
	author { J. Pino, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
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
	move	$ref2(input,phase),a
	move	a,$ref(output)
	}
	go {
		addCode(sendsample);
	}
	execTime {
		return 2;
	}
}
