defstar {
    name { Mux }
    domain { C50 }
    desc { Multiplexes any number of inputs onto one output stream. }
    version { $Id$ }
    author { Luis Gutierrez, based on CG56 version }
    acknowledge { SDF version by E. A. Lee, CG56 version by Kennard White }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { C50 control library }
    explanation {
.Id "multiplex"
\fIblockSize\fP particles are consumed on each input.
But only one of these blocks of particles is copied to the output.
The one copied is determined by the \fIcontrol\fP input.
Integers from $0$ through $N-1$ are accepted at the \fIcontrol\fP input,
where $N$ is the number of inputs.
If the \fIcontrol\fR input is outside this range, random data (possibly
memory mapped devices) will be copied.
.UH IMPLEMENTATION:
.pp
There are potentially very many special cases that could be handled
for increased efficiency: looped vs. non-looped, circular vs. linear,
and scalars vs vectors (\fIblockSize\fR > 1), uniform inputs vs. non-uniform
inputs (port.bufSize()).
The current implementation handles only some of these cases.
Use this star at your own risk.
.pp
At compile time, the star constructs a table of pointers to each of the
input blocks.
The \fIcontrol\fP input is used to index this table, yielding a pointer
to the appropriate input block for the firing.
.pp
Currently we advance each of the pointers in the table on every firing.
With some schedule the advancement is a nop; this case is handled.
In other schedules the advancement is periodic over all inputs; in this
case, we could pre-calculate a set of tables at compile time instead
of performing runtime advancement.
This is not currently handled.
    }
	inmulti {
		name {input}
		type {ANYTYPE}
		attributes {P_CIRC}
	}
	input {
		name {control}
		type {INT}
	}
	output {
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
		attributes { A_NONSETTABLE|A_UMEM|A_RAM|A_NOINIT|A_CIRC}
	}
	
	protected{
		int n;
		int iter;
	}

	initCode{
	  StringList ptrInit;
	  ptrInit << "\t.ds   $addr(ptrarray)\n";
	  for (int i=1; i<= n; i++){
	    ptrInit<<"\t.word  $addr(input#";
	    ptrInit<<i;
	    ptrInit<<")\n";
	  }
	  ptrInit<<"\ttext:\n";
	  addCode(ptrInit);
	}

	codeblock(loadAddress,""){
	lmmr	indx,#$addr(control)
	lar	ar1,#$addr(ptrarray)
	mar	*,ar1
	mar	*0+
	lacl	*,0,ar2
	samm	ar2		; ar2 -> start of current input
	}
	
	codeblock(moveInput,""){
	.if	@iter		; if iter > 0 use block xfer
	lacl	#$addr(output),0
	samm	bmar
	rpt	#@iter
	bldd	*+,bmar
	.else
	lacl	*,0
	lar	ar2,#$addr(output)			
	sacl	*,0
	.endif		
	}
	
	method {
		name { computeIterations }
		type { " void " }
		arglist { "()" }
		access { protected }
		code {
			iter = int(blockSize);
			if (output.resolvedType() == COMPLEX ){
				iter = 2*iter;
			};
			iter --;
		}
	}

	setup {
		n = input.numberPorts();
		ptrarray.resize(n);
		int portsize = int(blockSize);
		input.setSDFParams(portsize, portsize-1);
		output.setSDFParams(portsize, portsize-1);
		computeIterations();
	}
	
	go {
		addCode(loadAddress());
		addCode(moveInput());
	}
	exectime{
		int time = 6;
		if (iter){
			time += 4 + iter;
		} else {
			time += 3;
		}
		return time;
	}
}





