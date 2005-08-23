defstar {
    name { MpyShift }
    domain { CG56 }
    desc { Multiply and shift }
    version { @(#)CG56MpyShift.pl	1.12 03 Oct 1996 }
    author { Kennard White (ported from Gabriel) }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
    location { CG56 main library }
	htmldoc {
<a name="multiplication with shifting"></a>
Multiply inputs <i>factor1</i> and <i>factor2</i> together, and shift the
result left by <i>shift</i> bits.  The <i>shift</i> input must be zero
or positive; right (negative) shifts are not supported.
    }
    input {
	name {factor1}
	type {FIX}
    }
    input {
	name {factor2}
	type {FIX}
    }
    input {
	name {shift}
	type {INT}
    }
    output {
	name {output}
	type {FIX}
    }
    constructor {
	noInternalState();
    }
    go {
	addCode(cbMulShift);
    }
    codeblock (cbMulShift) {
	move	$ref(factor1),x0
	move	$ref(factor2),x1
	mpy	x0,x1,a		$ref(shift),b
	tst	b
	jle	$label(noleft)
	rep	b1
	  asl	a			; shift left by {shift} bits
$label(noleft)
	rnd	a			; Must do rnd *after* shift-left
	move	a,$ref(output)
    }
}
