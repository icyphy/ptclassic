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
Copyright (c) 1990-1996 The Regents of the University of California.
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

	// Inherit the setup method from the Chop star
	go {
		addCode(std());
	}
	
	codeblock(std,""){
	lar	ar0,#$addr(output)	; ar0 -> output
	lar	ar1,#$addr(offsetCntrl) ; ar1 -> offsetCntrl
	mar	*,ar1			; arp = 1
	lacc	#$val(nwrite),0		
	sacb				; accb = nwrite
	setc	sxm			; set sign xtension mode
	.if	@(int(C50Chop::cmplx))			
	lacc	*,1,ar0			; if input is cmplx double offset
	.else
	lacc	*,0,ar0			; accL = offsetCntrl
	.endif
	bcndd	$label(skpPd),leq	; delayed branch to avoid flushing
	samm	indx			; the pipeline.
	ldp	#00h			; branch taken here
	sach	indx,0			; clear index(since offset > 0)
	crlt				; accL = min( nwrite,max(offsetCntrl,1))
	sub	#1			; acc = padding - 1 
	samm	ar2			; store acc in ar2
	rpt	ar2
	sach	*+,0			; start padding with zeroes
$label(skpPd)				
	lamm	ar2			; acc = padding - 1
	add	#@(int(nread)+1)	; acc = padding + nread
	sacb				; accb = padding + nread
	lacc	#$val(nwrite),0		; acc = nwrite
	crlt				; acc = min(padding + nread,nwrite)
	sub	ar2,0			; acc = numCpy
	bcndd	$label(skpCp),leq	; if offset = nwrite don't copy input to output
	sub	#1			; delayed branch avoids flushing
	samm	ar3			; brach taken here
	lacc	#$addr(input)		; get addr of input
	sub	indx,0			; accL = start addr for cpy
	samm	bmar			; bmar = start addr for cpy
	rpt	ar3
	bldd	bmar,*+			; copy the inputs to output
$label(skpCp)
	lacc	#$val(nwrite),0
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
	// the value of the offset which is not known beforhand
		return 35 + int(nwrite);
	}
}
