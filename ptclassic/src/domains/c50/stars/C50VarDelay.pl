defstar {
	name { VarDelay }
	domain { C50 }
	desc { A variable delay with linear interpolation. }
	version { $Id$ }
	author { Luis Gutierrez, based on the CG56 version }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	explanation {
Delay will vary between 0 and \fImaxDelay\fR as the control varies
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
		attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_NOINIT}
	}
	state  {
		name { bufStart }
		type { int }
		default { 0 }
		desc { pointer to the buffer }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_NOINIT }
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
	.ds	$addr(bufStart)	; initialize internal buffer	
	.word	$addr(buf)   
	.text 	
	}			      

	codeblock(std,"") {
	ldp	#0
	lacl	#@(size - 1)
	samm	dbmr			; used for modulo 2^k addressing
	lmmr	ar1,#$addr(input)	; ar1 = most recent input
	lar	ar0,#$addr(bufStart)	; ar0 = pointer to start of buff.
	mar	*,ar0			; arp = 0
	smmr	ar1,*+			; store input on buffer, inc ar0
	apl	ar0			; get lowest k-1 bits
	opl	#$addr(bufStart),ar0	; add address; so ar0->nxt position
	smmr	ar0,#$addr(bufStart)	; store nxt postion
	lmmr	treg0,#$addr(control)	; treg0 = control
	lacc	#4000h,1		; 
	add	treg0,0			; acc = 0.5(control+1)*2
	sfr				; acc = 0.5(control+1)
	samm	treg0			; treg0 = 0.5(control+1)
	mpy	#$val(maxDelay)		; p = 0.5(maxDelay)*(0.5(control+1))
	pac				; acc = p
	add	#0003,14		; rnd up and add 1 more delay to ar0
	sach	indx,1			; indx = maxDelay*(0.5(control+1))+1
	mar	*0-			; ar0 = bufStart - indx
	apl	ar0			; get lowest k-1 bits
	opl	#$addr(buf),ar0		; ar0 = loc in buffer of element
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
		 return 23;
	}
}
