defstar {
	name { Quant }
	domain { CG56 }
	desc {
The star quantizes the input to one of N+1 possible output levels
using N thresholds.
	}
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	explanation {
The star quantizes the input to one of $N+1$ possible output levels
using $N$ thresholds.
For an input less than or equal to the $N$th threshold,
but larger than all previous thresholds,
the output will be the $N$th level.
If the input is greater than all thresholds,
the output is the $(N+1)$th level.
The \fIlevels\fR parameter must be one greater than the number
of \fIthresholds\fR.
	}
	input {
		name {input}
		type {fix}
	}
	output {
		name {output}
		type {fix}
	}
        state  {
                name { thresholds }
                type { fixarray }
                default { "0.1 0.2 0.3 0.4" }
                desc { threshold file }
                attributes { A_NONCONSTANT|A_XMEM }
        }
        state  {
                name { levels }
                type { fixarray }
                default { "0.05 0.15 0.25 0.35 0.45" }
                desc { levels file }
                attributes { A_NONCONSTANT|A_YMEM }
        }
        state  {
                name { X }
                type { int }
                default { 4 }
                desc { internal }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }
	constructor {
		noInternalState();
	}

	codeblock(main) {
; Register usage
; r0 = pointer into the array of thresholds
; r4 = pointer into the array of threshold levels
; r5 = address of the array of threshold levels (used by QuantIdx star)
; a = current threshold
; b = current threshold level
; x0 = input value
        move    #>$addr(levels),r4
        move    #<$addr(thresholds),r0
        move    r4,r5		$ref(input),x0
        move    x:(r0)+,a	y:(r4)+,b
        do      #$val(X)-1,$label(lab)		; begin loop
        cmp	x0,a
        jlt	$label(again)
        enddo				; jump out of loop
        jmp     $label(term)			; b is right threshold level
$label(again)
        move    x:(r0)+,a    y:(r4)+,b
$label(lab)				; end loop
        cmp	x0,a			; if input > threshold, then
	jge	$label(term)
	move    y:(r4)+,b
$label(term)
        move    b,$ref(output)		; output threshold level
	}

        codeblock(oneThreshold) {
; Register usage
; a = threshold			x0 = input
; b = first level value		y0 = second level value
	move	$ref(thresholds),a
	move	$ref(levels),b
        move    $ref(input),x0
	cmp	x0,a	$ref(levels)+1,y0	; compare input to threshold
	tge	y0,b		; if input >= threshold, update threshold
        move    b,$ref(output)
        }

        setup {
                int n = thresholds.size();

                if (levels.size() == 0) {       // set to default: 0, 1, 2...
                        levels.resize(n + 1);
                        for (int i = 0; i <= n; i++)
                                levels[i] = i;
                }
                else if (levels.size() != n+1) {
                        Error::abortRun (*this,
                              "must have one more level than thresholds");
                }
        }
	go {
                 X=thresholds.size();

                 if (thresholds.size()>1) 
                     addCode(main);
		 else
	             addCode(oneThreshold);
	}
	exectime {
		if ( thresholds.size() == 1 ) return 6;
	        return 12 + 4*int(thresholds.size());
	}
}
