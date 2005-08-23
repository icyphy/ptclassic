defstar {
	name { DeMux }
	domain { C50 }
	desc {
Demultiplexes one input onto any number of output streams.
The star consumes B particles from the input, where B is the blockSize.
These B particles are copied to exactly one output,
determined by the "control" input.  The other outputs get a zero of the
appropriate type.

Integers from 0 through N-1 are accepted at the "control"
input, where N is the number of outputs.  If the control input is
outside this range, all outputs get zero.
	}
	version { $Id$ }
	author { Luis Gutierrez }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location {  C50 control library }
	explanation {
.Id "demultiplex"
	}
	input {
		name {input}
		type {anytype}
	}
	input {
		name {control}
		type {int}
	}
	outmulti {
		name {output}
		type {anytype}
	}
	defstate {
		name {blockSize}
		type {int}
		default {1}
		desc {Number of particles in a block.}
	}
	defstate {
		name {ptrarray }
		type {INTARRAY }
		default {""}
		desc { Array of pointers to outputs }
		attributes { A_NONSETTABLE|A_UMEM|A_RAM|A_NOINIT|A_CIRC}
	}
	
	protected{
		int n;
		int inSize;
	}

	setup {
		n = output.numberPorts();
		ptrarray.resize(n);
	}

	initCode{
	  StringList ptrInit;
	  ptrInit << "\tds   $addr(ptrarray)\n";
	  for (int i=1; i<= n; i++){
	    ptrInit<<"\t.word  $addr(output#";
	    ptrInit<<i;
	    ptrInit<<")\n";
	  }
	  ptrInit<<"\ttext:\n";
	  addCode(ptrInit);
	}

	codeblock(loadAddress,""){
	ldp	#00h			; data page ptr = 0
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(ptrarray)	; ar1 -> ptrarray
	lmmr	indx,#$addr(control)	; index = control
	mar	*,ar1
	mar	*0+			; ar1 -> ptrarray[control]
	splk	#$addr(ptrarray),cbsr1	; load circ buff addr.
	splk	#$addr(ptrarray,@(n-1)),cber1
	splk	#0009h,cbcr		; enable circ buff 1 with ar1
	zap				; clear acc & p
	}
	
	codeblock(moveInput,"int iter"){
	.if	@iter
	lacl	*+,ar0
	samm	bmar
	rpt	#@iter
	bldd	*+,bmar
	mar	*,ar1
	.else
	lacl	*+,ar0
	samm	ar2			
	lacl	*,ar2
	sacl	*,ar1
	.endif		
	}
	
	codeblock(moveZero,"int iter"){
	.if	@iter
	lacl	*+,ar2
	samm	ar2
	rpt	#@iter
	sph	*+	
	mar	*,ar1
	.else
	lacl	*+,ar2
	samm	ar2
	sph	*,ar1
	.endif
	}

	codeblock(restore){
	splk	#0000h,cbcr		
	}

	go {
		int iter = (input.bufSize())*(int(blockSize));
		if (input.resolvedType() == COMPLEX){
			iter = 2*iter;
		}
		iter--;
		addCode(loadAddress());
		addCode(moveInput(iter));
		for (int i = 1; i<n; i++) addCode(moveZero(iter));
		addCode(restore);
	}
	exectime{
		int time = 10;
		if (int(blockSize) == 1) time+= (3*n + 1);
		else{ 
			time+= n*(int(blockSize)+4);
		}
		return time;
	}
}









