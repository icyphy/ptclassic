defstar {
	name { Sin }
	domain { CG56 }
	desc { 
Sine function.
Calculation by table lookup.  Input range of (-1,1) scaled by pi.
Output is sin(pi*input).
	}
	version { $Id$ }
	acknowledge { Gabriel version by Maureen O'Reilly }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
This star computes the sine of the input, which must be in the range
(-1.0, 1.0).
The output equals sin($~pi~cdot~$\fIin\fR),
so the input range is effectively (-$~pi$, $pi~$).
The output is in the range (-1.0, 1.0).
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
 	codeblock (main) {
	move    $ref(input),x0
; shift input 16 bits to the right and round --> a
        move    #>$$000080,x1
        mpyr    x0,x1,a         #>$$0000ff,x0
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
		gencode(main);
	}
	execTime {
		return 10;
	}
}
