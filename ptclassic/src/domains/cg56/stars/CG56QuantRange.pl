defstar {
    name { QuantRange }
    domain { CG56 }
    desc {
The star quantizes the input to one of N+1 possible output levels
using N thresholds.
    }
    version { @(#)CG56QuantRange.pl	1.7	01 Oct 1996 }
    author { Jose Luis Pino & Chih-Tsung Huang }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 main library }
	htmldoc {
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
    constructor {
	noInternalState();
    }

    codeblock (manyThresholds,"int numThresholds") {
	move	#<$addr(thresholds),r0
	move	#<$addr(levels),r4
        move    $ref(input),x0
	move	$ref(range),x1
	move 	x:(r0)+,y0
        do      #@numThresholds,$label(end)
	mpy	x1,y0,a		y:(r4)+,y1
        cmp	x0,a		x:(r0)+,y0
        jgt     $label(break)
	jmp 	$label(continue)
$label(break)
        enddo
	jmp	$label(done)
$label(continue)
	nop
$label(end)
	move	y:(r4)+,y1
$label(done)
	mpy	x1,y1,b
        move    b,$ref(output)
    }

    codeblock(oneThreshold) {
	move	$ref(range),x1
        move    $ref(thresholds),y0
	mpy	x1,y0,a		#<$addr(levels),r4
        move    $ref(input),x0
	cmp	x0,a		y:(r4)+,y1
	jgt     $label(done)
	move	y:(r4)+,y1
$label(done)
	mpy	x1,y1,b
        move    b,$ref(output)
    }

    setup {
	if(levels.size() != thresholds.size()+1)
	    Error::abortRun
		(*this,": Must have 1 more level than threshold to quantize.");
    }
    
    go {
	if(thresholds.size()>1) addCode(manyThresholds(thresholds.size()));
	else addCode(oneThreshold);
    }
    
    exectime {
	return 7+4*int(thresholds.size());
    }
}
