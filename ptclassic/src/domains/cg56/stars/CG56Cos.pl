defstar {
	name { Cos }
	domain { CG56 }
	desc { 
Cosine function calculated by table lookup.
Input range of (-1,1) scaled by pi.
	}
	version { @(#)CG56Cos.pl	1.18 06 Oct 1996 }
	acknowledge { Gabriel version by Maureen O'Reilly }
	author { J. Pino (ported from Gabriel) }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="cosine"></a>
This star computes the cosine of the input, which must be in the range
(-1.0, 1.0).
The output equals cos(<i>pi</i>&#183<i>input</i>), so the input range is 
effectively (<i>-pi</i>, <i>pi</i>).
The output is in the range (<i>-</i>1<i>.</i>0<i>, </i>1<i>.</i>0).
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	constructor {
		noInternalState();
	}
 	codeblock (main) {
	move    $ref(input),x0
; shift input 16 bits to the right and round --> a
        move    #>$$000080,x1
        mpyr    x0,x1,a         #>$$000040,y1
; add phase shift of 0.25 periods
        add     y1,a            #>$$0000ff,x0
; mask address bits --> r1
        and     x0,a            #>$$000100,x1
        or      x1,a
        move    a1,r1
; set DE=1 in OMR, to access sine tables
        ori     #$$04,omr	; turn on table
        andi    #$$fb,omr	; turn off table -- NOTE THE 1 CYCLE TIME LAG
				; CAUTION: Do not insert any instruction here
        move    y:(r1),b	; read the table
        move    b1,$ref(output)
	}
	go {
		addCode(main);
	}
	execTime {
		return 11;
	}
}
