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
	author { Kennard, Chih-Tsung Huang (ported from Gabriel) }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 demo library }
	explanation {
This star computes the sine of the input, which must be in the range
(-1.0, 1.0).
The output equals sin($~pi~cdot~$\fIin\fR$+\fIphase\fR),
so the input range is effectively (-$~pi$, $pi~$).
The output is in the range (-1.0, 1.0).
The parameter \fIphase\fR is in degrees (.e.g, cos() would use phase=90).
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	state {
		name {phase}
		type {FLOAT}
		default { "0" }
		desc { "Additive phase relative to the sin() function." }
	}
	state {
		name {phaseOffset}
		type {INT}
		default { "0" }
		desc { "Munged form of phase." }
		attributes { A_NONSETTABLE }
	}
 	codeblock (cbPrepare) {
	move    $ref(input),x0
; shift input 16 bits to the right and round --> a
        move    #>$$000080,x1
        mpyr    x0,x1,a		#>$val(phaseOffset),y1
;	add phase shift
	add	y1,a		#>$$0000ff,x0
; mask address bits --> r1
        and     x0,a            #>$$000100,x1
        or      x1,a
        move    a1,r1
	}
 	codeblock (cbTempTables) {
; set DE=1 in OMR, to access sine tables
; we use to play pipeline tricks with turning the sine tables on and off.
; However, this isnt safe in the presense of interupts, so we just eat
; the nop.
        ori     #$$04,omr	; turn on table  -- NOTE THE 1 CYCLE TIME LAG
	nop
        move    y:(r1),b	; read the table
        andi    #$$fb,omr	; turn off table -- NOTE THE 1 CYCLE TIME LAG
	nop
        move    b1,$ref(output)
	}

 	codeblock (cbPermTables) {
; We assume that DE=1 in OMR always, so no need to turn it on
        move    y:(r1),b	; read the table
        move    b1,$ref(output)
	}
	start {
		double ph = double(phase);
		if ( ph < 0 ) {
			int n = (int) floor(-ph / 360.0);
			ph += 360.0 * (n+1);
		} else if ( ph >= 360.0 ) {
			int n = (int) floor(ph / 360.0);
			ph -= 360.0 * n;
		}
		phaseOffset = (int) rint(ph/360.0*256.0);
	}
	go {
		addCode(cbPrepare);
		addCode(cbTempTables);
	}
	execTime {
		return 10;
	}
}
