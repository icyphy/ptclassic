defstar {
	name { IIDUniform }
	domain {C50}
	desc {
Generate pseudo-IID-uniform random variables.  The values range from
-range to range where "range" is a parameter.
	}
	version {@(#)C50IIDUniform.pl	1.6	05/26/98}
	author { A. Baensch, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	explanation {
.Id "uniform noise"
.Id "noise, uniform"
	}
	output {
		name { output }
		type { fix }
	}
	// FIXME: This state is incompatible with the SDF version.
	// The SDF version uses lower and upper as the states, so
	// this star is only valid when -lower = upper = range < 1.0
	defstate {
		name { range }
		type { fix }
		default { ONE }
		desc { range of random number generator is [-range,+range] }
		attributes { A_UMEM | A_CONSTANT | A_SETTABLE }	
	}
	defstate {
		name { seed }
		type { fix }
		default { 0.9877315592 }
		attributes { A_BMEM|A_NONSETTABLE|A_NONCONSTANT}
	}
	
// "common" part of random number generation
	codeblock(random) {
	mar	*,AR1
	lar	AR1,#$addr(seed)
	lar	AR2,#$addr(range)
	lar	AR7,#$addr(output)
	lacc	*,1
	xor	*
	sacl	INDX,2
	xor	INDX
	and	#8000h
	add	*,16
	sach	*,1
	}
// case where range=1: write state and output
	codeblock(range1) {
	mar	*,AR7			
	sach	*,1
	}
// case where range < 1: write state and scale output
	codeblock(rangeScale) {
	mar	*,AR2
	samm	TREG0
	mpy	*,AR7
	pac
	sach	*,1
	}
	go {
		addCode(random);
		if (range.asDouble() < C50_ONE) addCode(rangeScale);
		else addCode (range1);
	}
	execTime {
		return (range.asDouble() < C50_ONE) ? 32 : 27;
	}
}
