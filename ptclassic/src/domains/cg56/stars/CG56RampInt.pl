defstar {
	name { RampInt }
	domain { CG56 }
	desc { Integer Ramp Generator }
	version { @(#)CG56RampInt.pl	1.2	2/27/96 }
	author { Luis Javier Gutierrez and David G. Messerschmitt }
	copyright{
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, an disclaimer of warranty provisions.
	}
	location { CG56 main library }
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
		attributes{ A_YMEM|A_CONSTANT }
	}
	state {
		name { value }
		type { int }
		default { 0 }
		desc { initial value output by RampInt }
		attributes{ A_YMEM|A_CONSTANT }
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
		attributes { A_NONSETTABLE|A_YMEM|A_NONCONSTANT }
	}
	setup {
		sum = int(value);
	}
	go {
		addCode(init);
		addCode(add);
		if (int(saturation)) {
			addCode(saturate);
		}
		else {
			addCode(no_saturate);
		}
	}
        codeblock(init) {
	move	$ref(sum),a		; a holds the last sum
	move	$ref(step),x0		; x0 is the step
; can't use move #$val(step),x0 because move immediate instructions
; load 8 bit # in 8 most significant bits of X0 so if step is 8 
; X0 will be 10000..0 instead of 0...01000
        }
        codeblock(add) {
	add	x0,a	a,$ref(output)		; output last sum
        }
        codeblock(no_saturate) {
	move	a1,$ref(sum)
        }
	codeblock(saturate) {
	move	a,$ref(sum)
	}

	exectime{
		return 4;
	}
}
