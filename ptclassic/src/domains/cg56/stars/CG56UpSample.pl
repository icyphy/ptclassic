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
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
		attributes { P_CIRC }
	}
	state {
		name {factor}
		type {int}
		default {2}
		desc { Number of samples produced. }
		attributes { A_SETTABLE }
	}
	state {
	// Not Supported Yet
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
	codeblock (sendsample) {
	move	$ref(output),r1
	move	$size(output),m1
	move	$ref(input),x0
	move	$val(fill),a		x0,x:(r1)+
	}
	codeblock (repeatcode) {
	rep	#($val(factor)-1)
	}
	codeblock (fillcode) {
	move	a,x:(r1)+
	}
	go {
		gencode(sendsample);
		if (factor > 1) {
			if (factor > 2) {
				gencode(repeatcode);
			}
			gencode(fillcode);
		}
	}
	execTime {
		return int(factor) + 2 ;
	}
}
