defstar {
	name { Sqrt }
	domain { C50 }
	desc { Square Root. }
	version { $Id$ }
	author { Luis Gutierrez, based on the CG56 version }	
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	explanation {
Output is the square root of input. 
Negative values are equated to zero.
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
	codeblock(sqrtblock) {
	ldp	#00h		; data page ptr = 0
	spm	1		; set prod shift mode to 1
	lacl	#15
	samm	brcr		; setup block repeat cntr
	mar	*,ar1		; arp = 1
	lar	ar1,#$addr(input)
	lacc	*,16		; load acc with input
	sacl	arcr		; clear arcr
	xc	1,gt		; if input > 0 arcr = input
	sach	arcr		; else arcr = 0
	lar	ar0,#4000	; ar0 contains temporary result
	lacc	#4000,0		; acc = mask
	samm	dbmr		; dbmr = mask
	rptb	$label(sqrt)
	lt	ar0		; load treg with temp res.
	mpy	ar0		; p = squared res.
	sph	ar1		; ar1 = squared res.
	cmpr	2		; see of sqrd res > input
	sfr			; shift acc right
	xc	1,TC
	xpl	ar0	
	samm	dbmr
$label(sqrt)
	opl	ar0
	spm	0		; reset prod.shift mode
	smmr	ar0,#$addr(output)
	}

 	go {
 		addCode(sqrtblock);
 	}

	exectime {
		return 159;
 	}
}
