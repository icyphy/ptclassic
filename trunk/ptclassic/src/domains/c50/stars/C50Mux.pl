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
    state {
	name {blockSize}
	type {INT}
	default {1}
	desc {Number of particles in a block.}
    }
    state {
	name {useCircular}
	type {INT}
	default {1}
	desc { "Boolean: use circular addressing on inputs." }
    }
    state {
	name {ptrvec}
	type {INTARRAY}
	default {""}
	desc { "Array containing pointers to inputs and their lengths" }
	attributes {A_NONSETTABLE|A_NOINIT|A_UMEM}
    }
    state {
	name {curinput}
	type {INT}
	default {0}
	desc { "Current input within go() loop." }
	attributes {A_NONSETTABLE|A_NONCONSTANT}
    }
    state {
	name {useModuloB}
	type {INT}
	default {0}
	desc { "True if any input is not scalar." }
	attributes {A_NONSETTABLE|A_NONCONSTANT}
    }
    setup {
	output.setSDFParams(int(blockSize),int(blockSize)-1);
	input.setSDFParams(int(blockSize),int(blockSize)-1);
	if ( int(useCircular) ) {
	    input.setAttributes(P_CIRC);
	} else {
	    input.setAttributes(P_NONCIRC);
	}

	int np = input.numberPorts();
	ptrvec.resize(2*np);
    }
    initCode {
	StringList buff;
	int i, np = input.numberPorts();

	int allScalorB = TRUE;
	MPHIter portiter(input);
	for (i=0; i < np; i++) {
	    AsmPortHole *port = (AsmPortHole*) portiter++;
	    if ( port->bufSize() != 1 )
		allScalorB = FALSE;
	}
	useModuloB = int(useCircular) && ! allScalorB;

	buff<<"\t.ds	$addr(ptrvec)\n";
	addCode("\torg	x:$addr(ptrvec)\n");
	for (i=0; i < np; i++) {
	    buff<<"\t.word\t$addr(input#";
	    buff<<i+1;
	    buff<<")\n";
	}
	for ( i=0; i < np; i++) {
	    // There is a weirdness with scalar inputs: see cbCopy below
	    buff<<"\t.word\t$size(input#";
	    buff<<i+1;
	    buff<<")\n";
	}
	buff<<"\t.text\n";
	addCode(buff);
    }
    codeblock(cbCopyScalor) {
	lar	ar0,#$addr(ptrvec)
	lmmr	indx,#$addr(control)
	mar	*,ar0
	mar	*0+
	lacl	*
	samm	ar0
	bldd	*,#$addr(output)
    }
    codeblock(cbCopyBlock,"int offset,int iter") {
	lar	ar0,#$addr(ptrvec)
	lmmr	indx,#$addr(control)
	mar	*,ar0
	mar	*0+
	.if	$val(useModuloB)
	lacl	*,ar1		; acc = address of port
	samm	cbsr1		; cbsr1 = start address of port
	samm	ar1		; ar2 = start address of port
	lamm	ar0		; acc -> address of port
	add	#@offset,0	; acc -> end address of port
	samm	cber1		; cbsr1 = end address of port
	lacl	#09
	samm	cbcr		; set circ buff cntrl reg
	.else
	lacl	*,ar1
	samm	ar1		; ar2 = start address of port
	.endif
	rpt	#@iter
	bldd	*+,#$addr(output)
	.if	$val(useModuloB)
	zap
	samm	cbcr
	.endif
	}

    // the cbAdvancePtr is only code-gen'd when bufsize > 1.
    // note that ports are 1-based array, while states are 0-based
    codeblock(cbAdvancePtr,"int iter, int size") {
	; advance ptr for input#$val(curinput)
	lar	ar1,#$addr(ptrvec,curinput)
	mar	*,ar1
	.if	$val(useModuloB)
	ldp	#00h
	lacc	*
	lar	ar0,#@size
	samm	cbsr1
	add	ar0
	samm	cber1
	lacl	#09
	samm	cbcr
	.endif
	rpt	#@iter
	mar	*+
	smmr	ar1,#$addr(ptrvec,(curinput)
	.if	$val(useModuloB)
	splk	#00,cbcr
	.endif
	}


    go {
	if ( int(blockSize) == 1 )	addCode(cbCopyScalor);
	else				addCode(cbCopyBlock(int(ptrvec.size())/2,int(blockSize)-1));
	
	if ( int(useCircular) ) {
	    int np = input.numberPorts();
	    MPHIter portiter(input);
	    int iter, size;
	    iter = int(blockSize) - 1;
	    for (int i=0; i < np; i++) {
		AsmPortHole *port = (AsmPortHole*) portiter++;
		if ( port->bufSize() != int(blockSize) ) {
		    // The test above also catchs the bufSize==1 problem
		    curinput = i;
		    size = (port->bufSize()) - 1;
		    addCode(cbAdvancePtr(iter, size));
		}
	    }
	}

    }
}
