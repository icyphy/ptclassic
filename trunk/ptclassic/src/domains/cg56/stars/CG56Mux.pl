defstar {
    name { Mux }
    domain { CG56 }
    desc { Multiplexes any number of inputs onto one output stream. }
    version { $Id$ }
    author { Kennard White }
    acknowledge { SDF version by E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
    location { CG56 control library }
    explanation {
.Id "multiplex"
\fIblockSize\fP particles are consumed on each input.
But only one of these blocks of particles is copied to the output.
The one copied is determined by the \fIcontrol\fP input.
Integers from 0 through N-1 are accepted at the \fIcontrol\fP input,
where N is the number of inputs.  If the control input is outside
this range, random data (possibly memory mapped devices) will be copied.
.UH IMPLEMENTATION:
.pp
There are potentially very many special cases that could be handled
for increased efficiency: looped vs non-looped, circular vs linear,
and scalars vs vectors (blockSize > 1), 
uniform inputs vs non-uniform inputs (port.bufSize()).
The current implementation handles only some of these cases.  Use this
star at your own risk.
.LP
At compile time the star constructs a table of pointers to each of the
input blocks.  The \fIcontrol\fP input is used to index this table,
yielding a pointer to the appropriate input block for the firing.  This
implementation assumes that all of its input ports reside in X memory.
.LP
Currently we advance each of the pointers in the table on every firing.
With some schedule the advancement is a nop; this case is handled.
In other schedules the adancement is periodic over all inputs; in this
case, we could pre-calculate a set of tables at compile time instead
of performing run-time advancement.  This is not currently handled.
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
    state {
	name {blockSize}
	type {INT}
	default {1}
	desc {Number of particles in a block.}
    }
    state {
	name {ptrvec}
	type {INTARRAY}
	default {""}
	desc { "Array of pointers to inputs (x:) and lengths (y:)." }
	attributes {A_NONSETTABLE|A_SYMMETRIC|A_RAM|A_NOINIT}
    }
    state {
	name {curinput}
	type {INT}
	default {0}
	desc { "Current input within go() loop." }
	attributes {A_NONSETTABLE|A_NONCONSTANT}
    }
    state {
	name {notAllScalorB}
	type {INT}
	default {0}
	desc { "True if any input is not scalor." }
	attributes {A_NONSETTABLE|A_NONCONSTANT}
    }
    start {
	output.setSDFParams(int(blockSize),int(blockSize)-1);
	input.setSDFParams(int(blockSize),int(blockSize)-1);

	int np = input.numberPorts();
	ptrvec.resize(np);
    }
    initCode {
	char buf[100];
	int i, np = input.numberPorts();

	notAllScalorB = FALSE;
	MPHIter portiter(input);
	for (i=0; i < np; i++) {
	    AsmPortHole *port = (AsmPortHole*) portiter++;
	    if ( port->bufSize() != 1 )
		notAllScalorB = TRUE;
	}

	addCode("	org	x:$addr(ptrvec)");
	for (i=0; i < np; i++) {
	    sprintf( buf, "\tdc\t$addr(input#%d)", i+1);
	    addCode(buf);
	}
	addCode("	org	y:$addr(ptrvec)");
	for ( i=0; i < np; i++) {
	    // There is a weirdness with scalor inputs: see cbCopy below
	    sprintf( buf, "\tdc\t$size(input#%d)-1", i+1);
	    addCode(buf);
	}
	addCode("	org	p:");
    }
    codeblock(cbCopy) {
	move	#$addr(ptrvec),r0
	move	$ref(control),n0
	move	#$addr(output),r3
	move	x:(r0+n0),r2
	IF	$val(notAllScalorB)
	  move	y:(r0+n0),m2	; for scalors m2=0=fft, but doesnt matter
	ENDIF
	.LOOP	#$val(blockSize)
	  move	x:(r2)+,x0
	  move	x0,x:(r3)+
	.ENDL
	nop
    }
    // the cbAdvancePtr is only code-gen'd when bufsize > 1.
    // note that ports are 1-based array, while states are 0-based
    codeblock(cbAdvancePtr) {
	; advance ptr for input#$val(curinput)
	IF	$val(notAllScalorB)
	  move	#$size(input#curinput)-1,m2
	ENDIF
	move	x:$addr(ptrvec,curinput)-1,r2
	rep	#$val(blockSize)
	  move	(r2)+
	move	r2,x:$addr(ptrvec,curinput)-1
    }
    go {
	addCode(cbCopy);
	
	int np = input.numberPorts();
	MPHIter portiter(input);
	for (int i=0; i < np; i++) {
	    AsmPortHole *port = (AsmPortHole*) portiter++;
	    if ( port->bufSize() != int(blockSize) ) {
		// The test above also catchs the bufSize==1 problem
		curinput = i+1;
		addCode(cbAdvancePtr);
	    }
	}
	if ( int(notAllScalorB) ) {
	    addCode("	move	m7,m2");
	}
    }
}
