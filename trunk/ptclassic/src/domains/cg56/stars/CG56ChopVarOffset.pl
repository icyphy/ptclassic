defstar {
	name { ChopVarOffset }
	derivedFrom { Chop }
	domain { CG56 }
	desc {
This star has the same functionality as the
Chop star except now the offset parameter is
determined at run time through a control input.
	}
	version { $Id$ }
	author { Luis Javier Gutierrez }
	copyright{
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, an ddisclaimer of warranty provisions.
	}
	location { CG56 main library }

	input {
		name { offsetCntrl }
		type { int }
	}

	constructor {
		offset.setAttributes(A_NONCONSTANT | A_NONSETTABLE);
		use_past_inputs.setAttributes(A_NONCONSTANT | A_NONSETTABLE);
		use_past_inputs = FALSE;
	}

	// Inherit the setup method from the Chop star

	codeblock(init_range) {
; compute hiLim, index, and padding
	}

	codeblock(chop) {
; Register usage
; x0 = offset
; x1 = 1
; y0 = nread
; y1 = address of input buffer
; r5 = address of output buffer
; r0 = y1 + nread - 1
	move	$ref(offset),b
	clr	a	b,x0		; B=X0=offset	  
	neg	b	#$addr(output),r5		; B=-offset
	cmp	x0,a	$ref(nread),y0			; Y0=nread
	tgt	a,b			; if (offset > 0) B=0
	jle	$label(skip_padding)		; do does at least 1
	do	x0,$label(skip_padding)		; iteration so if offset
	move	a,$mem(output):(r5)+		; is 0 one most skip this
$label(skip_padding) 
	add	x0,a	$ref(one),x1		; A=offset X1=1
; Register usage change: x0 = nwrite
	add	y0,a	$ref(nwrite),x0		; A=offset+readn=hiLim X0=nwrite
	cmp	x0,a	$addr(input),y1
	tlt	x0,a			; if nwrite < hiLim, hiLim = nwrite
	cmp	y0,b
	jgt	$label(index_ok)		; if (index >= nread)
	move	y0,b			; index = nread-1
	sub	x1,b
$label(index_ok)
	add	y1,b
	move	b,r0
	do	a,$label(input_to_output)
	move	$mem(input):(r0+n0)+,x1		; copy input to output
	move	x1,$mem(output):(r5)+
$label(input_to_output)
	sub	x0,a			; A = hiLim-nwrite B=0
	tst	a			; if (hiLim-nwrite < 0)
	jge	$label(skip_end_padding)
	neg	a			; A = nwrite-hiLim
	do	a,$label(skip_end_padding)
	move	b,$mem(output):(r5)+	   
$label(skip_end_padding)
	}

        go {
		addCode(chop);
	}

	exectime {
		return CG56Chop::myExecTime() + 8;
	}
}
