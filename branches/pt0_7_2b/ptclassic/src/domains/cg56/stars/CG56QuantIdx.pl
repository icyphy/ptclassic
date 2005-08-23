defstar {
	name { QuantIdx }
	domain { CG56 }
	derived { Quant }
	desc {
The star quantizes the input to one of N+1 possible output levels
using N thresholds.  It also outputs the index of the quantization
level used.  See the Quant star for more information.
	}
	version { $Id$ }
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
        output {
		name {stepNumber}
		type {int}
		desc {Level of the quantization from 0 to N-1}
	}

	codeblock(outputStepNumber) {
; The quantization level is r4 - r5 - 1
	move	r5,y0	(r4)-
	move	r4,a
	sub	y0,a
	move	a,$ref(stepNumber)
	}

	codeblock(oneThreshold) {
; Register usage
; a = threshold                 x0 = input
; b = first level value         y0 = second level value
; r4 contains the value one
; r5 contains the index of the threshold, 0 or 1
	clr	b	move	$ref(thresholds),a
	move    $ref(input),x0	b,r4
	move	$ref(levels),b	b,r0	(r4)+
	cmp     x0,a    $ref(levels)+1,y0	; if input >= threshold, then
	tge     y0,b	r4,r0		; update threshold
	move    b,$ref(output)
	move	r0,$ref(stepNumber)
	}

	// Inherit the setup method

	go {
		if ( thresholds.size() == 1 ) {
		    addCode(oneThreshold);
		}
		else {
		    CG56Quant :: go();
		    addCode(outputStepNumber);
		}
	}

	exectime {
		if ( thresholds.size() == 1 ) return 7;
	        return 4 + CG56Quant::myExecTime();
	}
}
