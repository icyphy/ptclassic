defstar {
    name { MpyShift }
    domain { CG56 }
    desc { Multiply and shift }
    version { $Id$ }
    author { Kennard White (ported from Gabriel) }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
    location { CG56 library }
    explanation {
.Id "multiplication with shifting"
Multiply inputs \fIfactor1\fP and \fIfactor2\fP together, and shift the
result left by \fIshift\fP bits.  The \fIshift\fP input must be zero
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
