defstar {
	name { IIDUniform }
	domain {C50}
	desc {
Generate pseudo-IID-uniform random variables.  The values range from
-range to range where "range" is a parameter.
	}
	version { $Id$ }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 signal sources library }
	explanation {
.Id "uniform noise"
.Id "noise, uniform"
	}
	output {
		name { output }
		type { fix }
	}
	defstate {
		name { range }
		type { fix }
		default { ONE }
		desc { range of random number generator is [-range,+range] }
		attributes { A_UMEM|A_NOINIT|A_NONSETTABLE}	
}
	defstate {
		name { seed }
		type { int }
		default { 0 }
		attributes { A_UMEM|A_NOINIT|A_NONSETTABLE}
	}
	initCode {
		addCode(initSeed);
	}
	go {
		addCode(random);
		if (range.asDouble() < C50_ONE) addCode(rangeScale);
		else addCode (range1);
	}
	execTime {
		return (range.asDouble() < C50_ONE) ? 32 : 27;
	}
// "code" to initialize the seed
	codeblock(initSeed) {
	.ds	$addr(seed)
	.word	07e6dh
	.ds	$addr(range)
	.q15	$val(range)
	.text
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
}






