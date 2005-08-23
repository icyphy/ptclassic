defstar {
	name {Biquad}
	domain { C50 }
	desc {
This star generates code for a second order IIR filter (a "biquad").
Transfer function: (1 + n1*z^(-1) + n2*z^(-2))/(1 + d1*z^(-1) + d2*z^(-2))
With the default parameters, there is a pole at z=0.5 +/- j0.5, and no zero.
	}
	version { @(#)C50Biquad.pl	1.1	6/6/96 }
	author {  A.Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
	input {
		name { input }
		type { fix }
	}
	output {
		name { output }
		type { fix }
	}
// Note: order of state declarations is important!  d1, d2, n1, n2
// must be allocated in the order indicated.  The A_CONSEC attribute
// tells the memory allocator to do this right.
	state {
		name { d1 }
		type { fix }
		default { "-1.0" }
		attributes { A_ROM|A_BMEM|A_CONSEC }
	}
	state {
		name { d2 }
		type { fix }
		default { "-0.5" }
		attributes { A_ROM|A_BMEM|A_CONSEC }
	}
	state {
		name { n1 }
		type { fix }
		default { 0.0 }
		attributes { A_ROM|A_BMEM|A_CONSEC }
	}
	state {
		name { n2 }
		type { fix }
		default { 0.0 }
		attributes { A_ROM|A_BMEM }
	}
	state {
		name { state }
		type { fixarray }
		default { "0.0[2]" }
		desc { Filter states }
		attributes { A_RAM|A_UMEM|A_NONSETTABLE|A_NONCONSTANT }
	}
	codeblock (std) {
	zap					;clear P-Reg. and Accu
	mar	*,AR6
	lar	AR0,#$addr(d1)			;Address d1		=> AR0
	lar	AR5,#$addr(state)		;Address state		=> AR5
	lar	AR6,#$addr(input)		;Address input		=> AR6
	lar	AR7,#$addr(output)		;Address output		=> AR7
	lmmr	INDX,#$addr(state)		;val(state) => INDX
	lacc	*,15,AR5			;input => Accu
	lt	*+,AR0				;state => TREG0 incr AR5
	mpy	*+,AR5				;P-Reg=state1*d1 incr AR0
	sph	TREG0				;TREG0=state1*d1
	spm	0				;no shifts on P-Reg. output
	mpy	#-1.0				;P-Reg=-state1*d1
	lta	*-,AR0				;Accu=inp-st1*d1  TREG0=state2
	mpy	*+				;P-Reg=state2*d2 incr AR0
	spm	1				;1 left shift on P-Reg output
	sph	TREG0				;TREG0=state2*d2
	mpy	#-1.0				;P-Reg=-state2*d2
	lta	INDX				;Accu=Accu+st2*n2 TREG0=state1
	mpy	*+,AR5				;P-Reg=state1*n1 incr AR0
	apac					;Accu=Accu+P-Reg
	sach	*+,1				;Accu => new state1 incr AR5
	lt	*,AR0				;TREG0= state2
	mpy	*,AR7				;P-Reg=state2*n2
	apac					;Accu=Accu+state2*n2
	sach	*,1,AR5				;Accu => output
	lamm	INDX				;Accu = old state1
	sach	*,1				;new state2 = old state1
	}
	exectime { return 30;}
	go {
		addCode(std);
	}
}






	
