defstar {
	name { UpSample }
	domain { CG56 }
	desc { 
Upsample by a factor (default 2), filling with fill (default 0.0).  The
"phase" tells where to put the sample in an output block.  The default
is to output it first (phase = 0). The maximum phase is "factor" - 1.
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
	start {
		output.setSDFParams(int(factor),int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, ": phase must be < factor");
	}
	codeblock (initfill) {
; initialization code for star $fullname() - class CG56UpSample
	move	#$addr(output),r1
	move	#$val(fill),a
	}
	codeblock (repeatcode) {
	rep	#$size(output)
	}
	codeblock (fillcode) {
	move	a,x:(r1)+
	}
	initCode {
		gencode (initfill);
		if (factor > 1) gencode(repeatcode);
		gencode(fillcode);
	}
	codeblock (sendsample) {
	move	$ref(input),x0
	move	x0,$ref2(output,phase)
	}
	go {
		gencode(sendsample);
	}
	execTime {
		return 1;
	}
}
