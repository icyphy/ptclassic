defstar {
    name {SubInt}
    domain {CG56}
    desc { Output the "pos" input minus all "neg" inputs. }
    version { @(#)CG56SubInt.pl	1.3	9/16/96 }
    author { Brian L. Evans and Jose Luis Pino }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CGC main library }
    input {
	name {pos}
	type {int}
    }
    inmulti {
	name {neg}
	type {int}
    }
    output {
	name {output}
	type {int}
    }
    state {
        name { saturation }
	type { int }
	default { "YES" }
	desc { If true, use saturation arithmetic }
    }
    constructor {
	noInternalState();
    }
    code {
#define	CG56_REPEAT_THRESHOLD 3
    }
    codeblock(subtract) {
	move	$ref(pos),a
	move	$ref(neg#1),x0
	sub	x0,a
    }
    codeblock(startOp) {
	move	$addr(neg),r0
	move	$ref(pos),a
	move	(r0)+,x0
    }
    codeblock(repeat,"int n") {
	rep	#@n
    }
    codeblock(doOp) {
	sub	x0,a	(r0)+,x0
    }
    codeblock(finalDifference) {
	sub	x0,a
    }
    codeblock(saveResultSat) {
	move	a,$ref(output)
    }
    codeblock(saveResultNoSat){
	move	a1,$ref(output)
    }
    go { 
	int numNegInputs = neg.numberPorts();
	int numRepeats = numNegInputs - 1;

	if ( numNegInputs == 1 ) {
	    addCode(subtract);
	}
	else {
	    addCode(startOp);
	    // It takes 2 instruction cycles to set up the repeat loop
	    if ( numRepeats > CG56_REPEAT_THRESHOLD ) {
		addCode(repeat(numRepeats));
		addCode(doOp); 
	    }
	    else {
		for (int i = 1; i <= numRepeats; i++)
		    addCode(doOp); 
	    }
	    addCode(finalDifference);	
	}
	if (int(saturation)) addCode(saveResultSat);
	else addCode(saveResultNoSat);
    }
    exectime {
	int instructionCycles = 0;
	int numNegInputs = neg.numberPorts();
	int numRepeats = numNegInputs - 1;

	if ( numNegInputs == 1 )
	    instructionCycles = 4;
	else if ( numRepeats > CG56_REPEAT_THRESHOLD )
	    instructionCycles = 4 + 2 + 1 + numRepeats;
	else
	    instructionCycles = 4 + 1 + numRepeats;

	return neg.numberPorts() + 6;
    }
}
