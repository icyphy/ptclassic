defstar {
	name {Biquad}
	domain { CG56 }
	desc {
This star generates code for a second order IIR filter (a "biquad").
Transfer function: (1 + n1*z^(-1) + n2*z^(-2))/(1 + d1*z^(-1) + d2*z^(-2))
With the default parameters, there is a pole at z=0.5 +/- j0.5, and no zero.
	}
	version { $Id$}
	author { J. Buck, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
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
		attributes { A_ROM|A_XMEM|A_CONSEC }
	}
	state {
		name { d2 }
		type { fix }
		default { "-0.5" }
		attributes { A_ROM|A_XMEM|A_CONSEC }
	}
	state {
		name { n1 }
		type { fix }
		default { 0.0 }
		attributes { A_ROM|A_XMEM|A_CONSEC }
	}
	state {
		name { n2 }
		type { fix }
		default { 0.0 }
		attributes { A_ROM|A_XMEM }
	}
	state {
		name { state }
		type { fixarray }
		default { "0.0[2]" }
		desc { Filter states }
		attributes { A_RAM|A_YMEM }
	}
// code copied from Gabriel 56biquad star
	codeblock (std) {
	move	#$addr(d1),r0
	move	#$addr(state),r5
	move	$ref(input),a
	rnd	a		x:(r0)+,x0	y:(r5)+,y0
	mac	-x0,y0,a	x:(r0)+,x0	y:(r5),y1
	mac	-x0,y1,a	x:(r0)+,x0	y0,y:(r5)-
	mac	x0,y0,a		x:(r0)+,x0	a,y:(r5)
	mac	x0,y1,a
	move	a,$ref(output)
	}
	exectime { return 9;}
	go {
		addCode(std);
	}
}






	
