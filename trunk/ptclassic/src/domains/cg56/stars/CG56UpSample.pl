defstar {
	name { UpSample }
	domain { CG56 }
	desc { 
Upsample by a factor (default 2), filling with "fill" (default 0.0).  The
"phase" tells where to put the sample in an output block.  The default
is to output it first (phase of 0). The maximum phase is "factor" - 1.
	}
	version { @(#)CG56UpSample.pl	1.20   03/29/97 }
	author { Jose Luis Pino, ported from Gabriel }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {=input}
	}
	state {
		name {factor}
		type {int}
		default {2}
		desc { Number of samples produced. }
		attributes { A_SETTABLE }
	}
	state {
		name {phase}
		type {int}
		default {0}
		desc { Where to put the input in the output block. }
		attributes { A_SETTABLE }
	}
	state {
		name {fill}
		type {FIX}
		default {0.0}
		desc { Value to fill the output block. }
		attributes { A_SETTABLE }
	}
	setup {
		output.setSDFParams(int(factor),int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, ": phase must be < factor");
	}
	codeblock (initfill) {
	move	#$addr(output),r1
	move	#$val(fill),a
	}
	codeblock (repeatcode) {
	rep	#$size(output)
	}
	codeblock (fillcode) {
	move	a,$mem(output):(r1)+
	}
	initCode {
		addCode (initfill);
		if (int(factor) > 1) addCode(repeatcode);
		addCode(fillcode);
	}
	codeblock (sendsample) {
	move	$ref(input),a
	move	a,$ref2(output,phase)
	}
	go {
		addCode(sendsample);
	}
	execTime {
		return 2;
	}
}
