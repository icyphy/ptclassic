defstar {
	name { DeMux }
	domain { CG56 }
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
Copyright (c) 1995-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
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
		attributes { A_NONSETTABLE|A_YMEM|A_RAM|A_NOINIT|A_CIRC}
	}
	
	protected{
		int n;
	}

	setup {
		input.setSDFParams(int(blockSize),int(blockSize)-1);
		output.setSDFParams(int(blockSize),int(blockSize)-1);
		n = output.numberPorts();
		ptrarray.resize(n);
	}

	initCode{
	  StringList ptrInit;
	  ptrInit << "\torg   $ref(ptrarray)\n";
	  for (int i=1; i<= n; i++){
	    ptrInit<<"\tdc  $addr(output#";
	    ptrInit<<i;
	    ptrInit<<")\n";
	  }
	  ptrInit<<"\torg	p:\n";
	  addCode(ptrInit);
	}

	codeblock(loadAddress,""){
	move	x:$addr(control),n2	; n2 = control
	move	#>$addr(ptrarray),r2	; r2 = addr of ptrarray
	move	#@(n-1),m2		; initialize circ buffer for ptarray
	nop				; pipeline delay
	move	y:(r2)+n2,x0		; r2 -> ptarray[control]
	clr	b	#>$addr(input),r1
	move	y:(r2)+,r3
	}
	

	codeblock(moveInput,""){
@(int(blockSize)!= 1 ? "\tdo\t#$val(blockSize),$label(sendBlock)\n":"")\
	move	$mem(input):(r1)+,a
	move	a,$mem(output#1):(r3)+
@(int(blockSize)!= 1 ? "$label(sendBlock)\n":"")\
	}

	codeblock(moveZero,""){
	move	y:(r2)+,r3		; load addr of nxt output
@(int(blockSize)!= 1 ? "\trep\t#$val(blockSize)\n":"\tnop\n")\
	move	b,$mem(output#1):(r3)+
	}

	codeblock(restore){
	move	#$$ffff,m2		
	}

	go {
		addCode(loadAddress());
		addCode(moveInput());
		for (int i = 1; i<n; i++) addCode(moveZero());
		addCode(restore);
	}
	exectime{
		int time = 6;
		if (int(blockSize) == 1) time+= (2 + 2*(n-1) + 3);
		else{
			time+= (2*int(blockSize) + 3); 
			time+= (n-1)*(int(blockSize)+2);
		}
		return time;
	}
}
