defstar {
	name { RampInt }
	domain { CG56 }
	desc { Integer Ramp Generator }
	version {@(#)CG56RampInt.pl 1.10 1/26/96 }
	author { Luis Gutierrez }
	copyright{
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, an ddisclaimer of warranty provisions.
	}
	location { CG56 signal sources library }
	explanation {
Generates an integer ramp signal, starting at \fIvalue\fR (default 0)
with step size \fIstep\fR (default 1) saturating upon reaching largest integer
if \fIsaturation\R="YES"
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
		attributes{ A_YMEM|A_NONCONSTANT }
	}
	state {
		name { value }
		type { int }
		default { 0 }
		desc { initial value output by RampInt.}
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
	setup{
		sum=int(value);
	}
	go{
		addCode(init);
		addCode(add);
		if (int(saturation))
				addCode(saturate);
		else
				addCode(no_saturate);
	}
        codeblock(init){
                move $ref(sum),a
                move $ref(step),x0
        }
        codeblock(add){
                add x0,a  a,$ref(output)
        }
        codeblock(no_saturate){
                move a1,$ref(sum)
        }
	codeblock(saturate){
		move a,$ref(sum)
	}

	exectime{
		return 4;
	}
}
