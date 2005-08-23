defstar {
	name { VarDelay }
	domain { C50 }
	desc { A variable delay with linear interpolation. }
	version {@(#)C50VarDelay.pl	1.11	05/26/98}
	author { Luis Gutierrez, based on the CG56 version, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
Delay will vary between 0 and <i>maxDelay</i> as the control varies
between -1.0 and 1.0.
	}

	input  {
		name { input }
		type { fix }
	}
	input  { 
		name { control }
	  	type { fix }
	}
	output {
		name { output }
		type { fix }
	}
	state  {
		name { buf }
		type { fixarray }
		desc { buffer }
		default { "0" }
		attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_BMEM}
	}
	state  {
		name { bufStart }
		type { int }
		default { 0 }
		desc { pointer to the buffer }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_BMEM }
	}
	state  {
		name { maxDelay }
		type { int }
		default { 10 }
		desc { maximum delay }
	}

	private{
		int size;
	}

	codeblock(block) {
	  mar *,ar0
	  lar ar0,#$addr(buf)
	  sar ar0,$addr(bufStart)
	}			      

	codeblock(std,"") {
	ldp	#0
	lacc	#@(size - 1)
	setc	sxm			; set sign extension mode
	mar	*,ar2			; arp = 0
	samm	dbmr			; used for modulo 2^k addressing
	lmmr	ar2,#$addr(bufStart)	; ar2 = pointer to start of buff.
	lmmr	ar1,#$addr(input)	; ar1 = most recent input
	lamm	ar1
	sacl	*+,0			; store input on buffer, inc ar2
	apl	ar2			; get lowest k-1 bits
	opl	#$addr(buf),ar2		; add address; so ar2->nxt position
	smmr	ar2,#$addr(bufStart)	; store nxt postion
	lmmr	treg0,#$addr(control)	; treg0 = control
	lacc	#4000h,15		; 
	add	treg0,14		; acc = 0.5(control+1)*2
	sach	treg0,1			; treg0 = 0.5(control+1)
$starSymbol(delay)	.set	-$val(maxDelay)
	mpy	#$starSymbol(delay)	; p = -0.5(maxDelay)*(0.5(control+1))
	pac				; acc = p
	sub	#0001,15		; add 1 more delay to ar2
	add	ar2,15			; ar2 = ar2 - calculated delay
	sach	ar2,1			; store ar2
	apl	ar2			; get lowest k-1 bits
	opl	#$addr(buf),ar2		; ar2 = loc in buffer of element
	bldd	*,#$addr(output)	; output element
	}

	setup {
		size = 1;
		if (int(maxDelay) < 0) Error::abortRun(*this,
			"Delay can not be negative!!");
//the C50 does not support modulo addressing so the easiest 
//way of implementing this star is using a buffer of size 2^k
//even though this wastes memory.
		for (int i = 0; i<16; i++) {
			if (size >= int(maxDelay)) break;
			size = size<<1;
		};
		buf.resize(size);
	}		
	initCode {
		addCode(block);
	}
	go {
		addCode(std());
	}		

	execTime { 
		 return 24;
	}
}
