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
	version {@(#)C50DeMux.pl	1.7	05/26/98}
	author { Luis Gutierrez, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location {  C50 control library }
	htmldoc {
<a name="demultiplex"></a>
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
		type {=input}
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
		attributes { A_NONSETTABLE|A_BMEM|A_CIRC|A_NONCONSTANT}
	}
	
	protected{
		int n;
		int iter;
	}

	initCode{
	  StringList ptrInit;
	  ptrInit << "\t mar *,ar0\n";
	  for (int i=1; i<= n; i++){
	    ptrInit << "\t lar ar0,#$addr(output#" << i << ")\n"
	            << "\t sar ar0,($addr(ptrarray)+" << (i-1) << ")\n";
	  }
	  addCode(ptrInit);
	}

	codeblock(loadAddress,""){
	ldp	#00h
	splk	#$addr(ptrarray),cbsr1	; load circ buff addr.
	splk	#($addr(ptrarray)+@(n-1)),cber1
	splk	#0009h,cbcr		; enable circ buff 1 with ar1
	lar	ar1,#$addr(ptrarray)	; ar1 -> ptrarray
	lmmr	indx,#$addr(control)	; index = control
	mar	*,ar1
	nop
	zap
	mar	*0+			; ar1 -> ptrarray[control]
	}
	
	codeblock(moveInput,""){
	lar	ar3,*+,ar3
	rpt	#@iter
	bldd	#$addr(input),*+
	mar	*,ar1
	}
	
	codeblock(moveOne){
	lar	ar3,*+,ar3
	bldd	#$addr(input),*
	mar	*,ar1
	}
	
	codeblock(moveZero,""){
	lar	ar2,*+,ar2
	rpt	#@iter
	sacl	*+
	mar	*,ar1
	}
	
	codeblock(moveOneZero){
	lar	ar2,*+,ar2
	sacl	*
	mar	*,ar1
	}

	codeblock(restore){
	splk	#0000h,cbcr	; deactivate circ. buffer addressing	
	}

	method {
		name { computeIterations }
		type { " void " }
		arglist { "()" }
		access { protected }
		code {
			iter = int(blockSize);
			if (input.resolvedType() == COMPLEX ){
				iter = 2*iter;
			};
			iter --;
		}
	}

	setup {
		n = output.numberPorts();
		ptrarray.resize(n);
		int portsize = int(blockSize);
		input.setSDFParams(portsize, portsize-1);
		output.setSDFParams(portsize, portsize-1);
		computeIterations();
	}


	go {
		addCode(loadAddress());
		if (iter) 
			addCode(moveInput());
		else
			addCode(moveOne);	
		for (int i = 1; i<n; i++) {
			if (iter) 
				addCode(moveZero());
			else
				addCode(moveOneZero);
		}
		addCode(restore);
	}

	exectime{
		int time = 10;
		if ( iter ) {
			time+= n*(8 + iter);
		} else{ 
			time+= n*3 + 1;
		}
		return time;
	}
}









