defstar {
    name { Skew }
    domain { CG56 }
    desc { Generic skewing star. }
    version { $Id$ }
    author { Kennard White }
    copyright { 1991 The Regents of the University of California }
    location { CG56 library }
    explanation {
The \fIerror\fP input is accumulated.  Normally the constant 0.0 is output.
However, when overflow occurs, \fIoverVal\fP is output, 
and when underflow occurs, \fIunderVal\fP is output.
    }
    seealso {SSISkew}
    input {
	name { error }
	type { FIX }
    }
    output {
	name { output }
	type { FIX }
    }
    state {
	name { accum }
	type { FIX }
	default { 0.5 }
	desc { Accumulation of error skew. }
	attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM }
    }
    state {
	name { underflowVal }
	type { FIX }
	default { "-0.9" }
	desc { Value to output on underflow. }
    }
    state {
	name { overflowVal }
	type { FIX }
	default { "0.9" }
	desc { Value to output on overflow. }
    }
    codeblock(cbSkew) {
	move	$ref(accum),a
	clr	b	$ref(error),x0
	add	x0,a	#0.5,x1
	move	#$val(overflowVal),y1
	move	#$val(underflowVal),y0
	tes	x1,a
	tes	y1,b
	tmi	x1,a
	tmi	y0,b
	move	a1,$ref(accum)
	move	b1,$ref(output)
    }
    go {
	addCode(cbSkew);
    }
}
