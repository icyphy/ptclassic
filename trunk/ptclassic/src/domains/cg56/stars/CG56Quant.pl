defstar {
	name { Quant }
	domain { CG56 }
	desc { The star quantizes the input to one of N+1 possible output levels
using N thresholds.
	}
	version { $Id$ }
	author { Chih-Tsung Huang }
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

	codeblock (main) {
        move    #<$addr(thresholds),r0
        move    #>$addr(levels),r4
        move    $ref(input),x0
        move    x:(r0)+,a    y:(r4)+,b
        do      #$val(X)-1,$label(lab)
        cmp	x0,a
        jlt	$label(again)
        enddo
        jmp     $label(term)
$label(again)
        move    x:(r0)+,a    y:(r4)+,b
$label(lab)
        cmp	x0,a
        jge	$label(term)
        move    y:(r4),b
$label(term)
        move    b,$ref(output)
	}

        codeblock(other) {
        move    #<$addr(thresholds),r0
        move    #>$addr(levels),r4
        move    $ref(input),x0
        move    x:(r0),a     y:(r4)+,b
        cmp	x0,a
        jge	$label(term)
        move    y:(r4),b
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
