defstar {
	name { Repeat }
	domain { C50 }
	desc { Repeats each input sample the specified number of times. }
	version {@(#)C50Repeat.pl	1.9	05/26/98}
	author { Luis Gutierrez, A. Baensch, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
Repeat repeats each input Particle the specified number of times
(<i>numTimes</i>) on the output.  Note that this is a sample rate
change, and hence affects the number of invocations of downstream
stars.
<h3>IMPLEMENTATION</h3>
We must be careful to avoid large interrupt latencies.  To be optimal
this requires the aid of the target.  For now, we just assume the
worst case.
	}
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {=input}
	}

	state {
		name {blockSize}
		type { int }
		default { 1 }
		desc { Number of particles in a block }
	}

	state {
		name {numTimes}
		type {int}
		default {2}
		desc { Repetition factor }
	}

	protected {
		//effBlockSize hold the number of words per block
		int effBlockSize;
	}

	setup {
		if (input.resolvedType() == COMPLEX ) 
			effBlockSize = 2*int(blockSize);
		else 
			effBlockSize = int(blockSize);
		int reps = int(numTimes)*int(blockSize);
		input.setSDFParams(int(blockSize), int(blockSize)-1);
		output.setSDFParams(reps, reps - 1);
	}

	codeblock(cbOnce) {
	lmmr	ar1,#$addr(input)
	nop
	nop
	smmr	ar1,#$addr(output)
	}

	codeblock(cbOnceCx){
	lmmr	ar1,#$addr(input)
	lmmr	ar2,#($addr(input)+1)
	smmr	ar1,#$addr(output)
	smmr	ar2,#($addr(output)+1)
	}

	codeblock(initRepLoop,""){
	lacc	#$addr(input)
	samm	cbsr1
	samm	ar1
	add	#@(effBlockSize-1)
	samm	cber1
	lacl	#9
	samm	cbcr	; set up circ buff. addrsd by ar1
	mar	*,ar1
	}

	codeblock(cbRepLoop,"") {
	rpt	#@(effBlockSize*int(numTimes)-1)
	bldd	*+,#$addr(output)
	zap
	samm	cbcr	; disable circ buffer
	}

	go {

		if ((int(numTimes) == 1) && (effBlockSize == 1) ) 
			addCode(cbOnce);
		else if ((int(numTimes) == 1) && (effBlockSize == 2))
			addCode(cbOnceCx);
		else {
			addCode(initRepLoop());
			addCode(cbRepLoop());
		}
	}

	exectime {
		if ((int(numTimes)==1) && (effBlockSize == 1))
			return 2;
		else if ((int(numTimes)==1) && (effBlockSize == 2))
			return 4;
		else 
			return (11 + (effBlockSize*int(numTimes)));
	}
}
