defstar {
	name { QuantRange }
	domain { CG56 }
	desc { The star quantizes the input to one of N+1 possible output levels
using N thresholds.
	}
	version { $Id$ }
	author { Chih-Tsung Huang & Jose Luis Pino}
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 nonlinear functions library }
	explanation {
.PP
The star quantizes the input to one of N+1 possible output levels
using N thresholds.
For an input less than or equal to the Nth threshold,
but larger than all previous thresholds,
the output will be the Nth level.
If the input is greater than all thresholds,
the output is the (N+1)th level.
There must be one more level than thresholds.
	}
	input {
		name {input}
		type {fix}
	}
	input {
		name {range}
		type {fix}
	}
	output {
		name {output}
		type {fix}
	}
        state  {
                name { thresholds }
                type { fixarray }
                default { "0" }
                desc { threshold values }
                attributes { A_NONCONSTANT|A_XMEM }
        }
        state  {
                name { levels }
                type { fixarray }
                default { "-0.5 0.5" }
                desc { levels values }
                attributes { A_NONCONSTANT|A_YMEM }
        }
        state  {
                name { X }
                type { int }
                default { 4 }
                desc { internal }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }

	codeblock (main) {
        move    #<$addr(thresholds),r0
        move    #>$addr(levels),r4
        move    $ref(input),x0
	move	$ref(range),x1
        move    x:(r0)+,y0
	move	y:(r4)+,y1
        do      #$val(X)-1,$label(lab)
	mpy	x1,y0,a
	mpy	x1,y1,b
        cmp	x0,a
        jlt	$label(again)
        enddo
        jmp     $label(term)
$label(again)
        move    x:(r0)+,y0
	move	y:(r4)+,y1
$label(lab)
        cmp	x0,a
        jge	$label(term)
        move    y:(r4),y1
	mpy	x1,y1,b
$label(term)
        move    b,$ref(output)
	}

        codeblock(other) {
        move    #<$addr(thresholds),r0
        move    #>$addr(levels),r4
        move    $ref(input),x0
	move	$ref(range),x1
        move    x:(r0),y0
	move	y:(r4)+,y1
	mpy	x1,y0,a
	mpy	x1,y1,b
        cmp	x0,a
        jge	$label(term)
        move    y:(r4),y1
	mpy	x1,y1,b
$label(term)
        move    b,$ref(output)
        }
        setup {
             if(levels.size() != thresholds.size()+1)
                 Error::abortRun (*this,
		 ": Must have 1 more level than threshold to quantize.");
        }
	go {
                 X=thresholds.size();

                 if(thresholds.size()>1) 
                     addCode(main);
		 else
	             addCode(other);
	}
	exectime {
	        return 7+4*int(thresholds.size());
	}
}
