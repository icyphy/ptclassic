defstar {
	name { MpyShift }
	domain { C50 }
	desc { Multiply and shift }
	version { @(#)C50MpyShift.pl	1.4	03 Oct 1996 }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
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
	mar	*,AR1
	lar	AR1,#$addr(factor1)		;Address factor1	=> AR1
	lar	AR2,#$addr(factor2)		;Address factor2	=> AR2
	lar	AR3,#$addr(shift)		;Address shift		=> AR3
	lar	AR7,#$addr(output)		;Address output		=> AR7
	lt	*,AR2				;TREG0 = factor1
	mpy	*,AR3				;P-Reg.=factor1*factor2
	lacc	*,AR7				;Accu = number of shifts
	bcnd	$label(nolf),EQ			;if Accu =0 jump to label(nolf)
	pac					;Accu = factor1*factor2
	rpt	#$val(shift)			;
	sfl					;shift left by {shift} bits
$label(nolf)					;
	sach	*,1				;output = Accu
	}
}
