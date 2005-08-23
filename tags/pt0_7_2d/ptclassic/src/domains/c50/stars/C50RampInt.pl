defstar {
	name { RampInt }
	domain { C50 }
	desc { Integer Ramp Generator }
	version {@(#)C50RampInt.pl	1.5	06 Oct 1996}
	author { Luis Javier Gutierrez}
	copyright{
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, an ddisclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
Generates an integer ramp signal, starting at <i>value</i> (default 0)
with step size <i>step</i> (default 1) saturating upon reaching largest
integer if <i>saturation</i> is "YES".
	}
	output {
		name { output }
		type { int }
	}
	state {
		name { step }
		type { int }
		default { 1 }
		desc { Increment from one sample to the next }
		attributes{ A_UMEM|A_CONSTANT }
	}
	state {
		name { value }
		type { int }
		default { 0 }
		desc { initial value output by RampInt }
		attributes{ A_SETTABLE }
	}
	state {
		name { saturation }
		type { int }
		default { "YES" }
		desc { if set to YES uses saturation arithmetic }
	}
	state {
		name { sum }
		type { int }
		default { 0 }
		desc { internal accumulator }
		attributes { A_NONSETTABLE|A_UMEM|A_NONCONSTANT }
	}
	setup {
		sum = int(value);
	}
	go {
		if (int(saturation)) {
			addCode(saturate);
		}
		addCode(accumulate);
		if (int(saturation)) {
			addCode(clearSaturation);
		}
	}

	codeblock(saturate){
	setc	ovm
	}


	codeblock(clearSaturation){
	clrc	ovm
	}

        codeblock(accumulate) {
	lar	ar0,#$addr(output)
	lar	ar1,#$addr(sum)
	lar	ar2,#$addr(step)
	mar	*,ar1
	lacc	*,16,ar0
	sach	*,0,ar2
	add	*,16,ar1
	sach	*,0
	}

	exectime{
		return 8;
	}
}







