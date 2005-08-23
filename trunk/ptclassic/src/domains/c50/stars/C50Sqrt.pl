defstar {
	name { Sqrt }
	domain { C50 }
	desc { Square Root. }
	version { @(#)C50Sqrt.pl	1.4	01 Oct 1996 }
	author { Luis Gutierrez, based on the CG56 version }	
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
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
	lacl	#14
	samm	brcr		; setup block repeat cntr
	mar	*,ar1		; arp = 1
	lmmr	arcr,#$addr(input) ; arcr = input
	lar	ar2,#4000h	; ar2 contains temporary result
	lacc	#4000h,0	; acc = mask
	samm	dbmr		; dbmr = mask
	rptb	$label(sqrt)
	lt	ar2		; load treg with temp res.
	mpy	ar2		; p = squared res.
	sph	ar1		; ar1 = squared res.
	nop			; nop's needed so that
	nop			; ar1 has the right value for cmpr
	cmpr	2		; see of sqrd res > input
	sfr			; shift acc right
	xc	1,TC
	xpl	ar2	
	samm	dbmr
$label(sqrt)
	opl	ar2
	lacc	arcr,16		; result o.k as long as
	spm	0		; input is not <= 0
	xc	1,leq		; handle this case
	lar	ar2,#0h		; here
	smmr	ar2,#$addr(output)
	}

 	go {
 		addCode(sqrtblock);
 	}

	exectime {
		return 191;
 	}
}
