defstar {
	name { ChopVarOffset }
	derivedFrom { Chop }
	domain { C50 }
	desc {
This star has the same functionality as the Chop star except now the
offset parameter is determined at run time through a control input.
	}
	version { $Id$ }
	author { Luis Javier Gutierrez }
	copyright{
Copyright (c) 1996-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, an disclaimer of warranty provisions.
	}
	location { C50 main library }

	input {
		name { offsetCntrl }
		type { int }
		desc { controls the offset parameter in the Chop star }
	}

	constructor {
		pastInputs.clearAttributes(A_UMEM);		
		offset.setAttributes(A_NONCONSTANT | A_NONSETTABLE);
		use_past_inputs.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
		use_past_inputs = FALSE;
	}

	// don't want oto inherit initCode from Chop so declare a dummy 
	// initCode here
	
	initCode{
	}

	// Inherit the setup method from the Chop star
	go {
		addCode(std());
	}
	
	codeblock(std,""){
	lar	ar4,#$addr(output)	; ar4 -> output
	lar	ar1,#$addr(offsetCntrl) ; ar1 -> offsetCntrl
	lar	ar5,#0			; ar5 = effective padding
	mar	*,ar1			; arp = 1
	lacc	#@(write),0		
	sacb				; accb = nwrite
	setc	sxm			; set sign extension mode
	.if	@(int(C50Chop::cmplx))			
	lacc	*,1,ar4			; if input is cmplx double offset
	.else
	lacc	*,0,ar4			; accL = offsetCntrl
	.endif
	samm	ar6			; ar6 = effective offset
	bcndd	$label(skpPd),leq	; delayed branch to avoid flushing
	samm	indx			; the pipeline.
	ldp	#00h			; branch taken here
	sach	indx,0			; clear index(since offset > 0)
	crlt				; accL = min( nwrite,max(offsetCntrl,1))
	samm	ar5			; ar5 = effective padding
	sub	#1			; acc = padding - 1 
	samm	ar2			; store acc in ar2
	rpt	ar2
	sach	*+,0			; start padding with zeroes
$label(skpPd)				
	lacc	ar6			; acc = eff offset
	add	#@(read)		; acc = offset + nread
	sacb				; accb = padding + nread
	lacc	#@(write),0		; acc = nwrite
	crlt				; acc = min(padding + nread,nwrite)
	sub	ar5,0			; acc = numCpy
	bcndd	$label(skpCp),leq	; if offset = nwrite don't copy input to output
	sub	#1			; delayed branch avoids flushing
	samm	ar3			; brach taken here
	lacc	#$addr(input)		; get addr of input
	sub	indx,0			; accL = start addr for cpy
	samm	bmar			; bmar = start addr for cpy
	rpt	ar3
	bldd	bmar,*+			; copy the inputs to output
$label(skpCp)
	lacc	#@(write),0
	sbb				; acc = nwrite - (padding + nread)
	zpr				; preg = 0
	bcndd	$label(skpTrl),leq	; (nwrite - (padding + nread)) <= 0 ? 
	sub	#1			; delayed branch.
	samm	ar3			; brach taken here
	rpt	ar3
	sph	*+
$label(skpTrl)
	}
		
	exectime{
	// The execution time returned is the maximum execution time for 
	// the star since the actual execution time is determined by 
	// the value of the offset which is not known beforehand
		return 41 + int(nwrite);
	}
}
