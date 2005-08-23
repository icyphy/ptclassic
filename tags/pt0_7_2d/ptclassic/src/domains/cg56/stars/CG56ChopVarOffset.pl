defstar {
	name { ChopVarOffset }
	derivedFrom { Chop }
	domain { CG56 }
	desc {
This star has the same functionality as the Chop star except now the
offset parameter is determined at run time through a control input.
	}
	version { @(#)CG56ChopVarOffset.pl	1.4	3/10/96 }
	author { Luis Javier Gutierrez }
	copyright{
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, an disclaimer of warranty provisions.
	}
	location { CG56 main library }

	input {
		name { offsetCntrl }
		type { int }
		desc { controls the offset parameter in the Chop star }
	}

	constructor {
		offset.setAttributes(A_NONCONSTANT | A_NONSETTABLE);
		// nread and nwrite need to be allocated in memory.
		// one can't use move #$val(nwrite),{x0,a,b} since this
		// puts an 8-bit constant (the value of nwrite) into the
		// 8 MOST significant bits of {x0,a,b}. move $ref(nwrite),x0 
		// does the right thing.
		nread.setAttributes(A_CONSTANT|A_YMEM);
		nwrite.setAttributes(A_CONSTANT|A_YMEM);
		use_past_inputs.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
		use_past_inputs = FALSE;
	}

	// Inherit the setup method from the Chop star
	go {
		addCode(initRange);
		addCode(chop);
	}

	codeblock(initRange) {
; Register usage:
; a  = temporary data
; b  = # of 0's to pad at end
; x0 = # of 0's to pad at beginning
; x1 = # of inputs written to output
; y1 = temporary data
; r0 = address of first input written to output
; n0 = # of inputs to skip
; r5 = address of first output
; if offset < 0 and |offset| > nread no inputs will be written
	move	$ref(offsetCntrl),a
	move	#0,x0
	move	$ref(nread),x1			; x1 = nread
	neg	a	a,b		; a = -offset  b = offset
	tst	a	$ref(nwrite),y0
	tlt	x0,a			; if offset > 0 a=0 
	tgt	x0,b			; if offset < 0 b=0 
	cmp	x1,a			; check num of inputs to skip > nread;
	tgt	x1,a			; if so, num of inputs to skip = nread
	cmp	y0,b	a,y1		; check padding > nwrite
	tgt	y0,b			; if so, padding = nwrite
	move	b,x0
	move	x1,b
; b = # of inputs that can be read = nread - num of inputs to skip
; a = number of outputs that weren't padded with 0's(i.e writable)
	sub	y1,b	y1,n0
	move	$ref(nwrite),a
	sub	x0,a	(r0)+n0
	cmp	b,a
; x1 = # of inputs to read; a = # inputs to read + # of padding 0's
	tge	b,a			; a = # inputs to read
	add	x0,a	a,x1
; b = nwrite - # inputs to read - # of padding 0's = # of padding 0's at end	
	move	$ref(nwrite),b
	sub	a,b	#$addr(output),r5
	}
	
	codeblock(chop) {
; avoid looping by 0 because 0 means to loop 65536 times
	clr	a				; a = 0
	cmp	x0,a	b,y0
	jge	$label(skip_initial_padding)
	rep	x0
	move	a,$mem(output):(r5)+			; insert 0's
$label(skip_initial_padding)
	cmp	x1,a
	jge	$label(skip_copying_input_to_output)
	do	x1,$label(skip_copying_input_to_output)
	move	$mem(input):(r0)+,y1
	move	y1,$mem(output):(r5)+
$label(skip_copying_input_to_output)
	cmp	y0,a
	jge	$label(skip_padding_at_end)
	rep	y0
	move	a,$mem(output):(r5)+
$label(skip_padding_at_end)
	}

	exectime{
	// can't determine the execution time since the offset is variable
	// return the time to do condition checking(which is fixed) +
	// time to execute chop assuming no padding is done and (nwrite)
	// inputs are written
		return 21 + 20 + 2*int(nwrite);
	}
}
