defstar {
	name { Logic }
	domain { CG56 }
	version { @(#)CG56Logic.pl	1.18	01 Oct 1996 }
	author { Brian L. Evans }
	acknowledge { Edward A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	desc {
This star applies a logical operation to any number of inputs.
The inputs are integers interpreted as Boolean values,
where zero means FALSE and a nonzero value means TRUE.
The logical operations supported are {NOT AND NAND OR NOR XOR XNOR}.
	}
	htmldoc {
The NOT operation requires that there be only one input.
The XOR operation with multiple inputs tests for an odd number
of TRUE values among the inputs.
The other operations are self-explanatory.
	}
	inmulti {
		name { input }
		type { int }
		desc { Input logic values. }
	}
	output {
		name { output }
		type { int }
		desc {
Result of the logic test, with FALSE equal to zero and TRUE equal to a
non-zero integer (not necessarily 1).
		}
	}
	defstate {
		name { logic }
		type { string }
		default { "AND" }
		desc { The test logic: one of NOT AND NAND OR NOR XOR or XNOR. }
	}
	code {
#define NOTID 0
#define ANDID 1
#define NANDID 2
#define ORID 3
#define NORID 4
#define XORID 5
#define XNORID 6
	}
	private {
		int test;
	}
        constructor {
		noInternalState();
		test = NOTID;
	}
	setup {
		const char* cn = logic;
		if ( strcasecmp(cn, "NOT") == 0) {
		    test = NOTID;
		    if (input.numberPorts() > 1)
			Error::abortRun(*this,
			    "NOT operation can only have one input");
		}
		else if ( strcasecmp(cn, "AND") == 0 ) test = ANDID;
		else if ( strcasecmp(cn, "NAND") == 0 ) test = NANDID;
		else if ( strcasecmp(cn, "OR") == 0 ) test = ORID;
		else if ( strcasecmp(cn, "NOR") == 0 ) test = NORID;
		else if ( strcasecmp(cn, "XOR") == 0 ) test = XORID;
		else if ( strcasecmp(cn, "XNOR") == 0 ) test = XNORID;
		else Error::abortRun(*this, "Unrecognized test.");
	}

	// Single-input cases

	codeblock(inverter) {
	clr	b	$ref(input#1),a		; b = 0, a = input#1
	tst	a	#$$01,y1	; test a, set y1 = 1
	teq	y1,b			; if a = 0, then b = y1 = 1
	move	b,$ref(output)			; output = b
	}

	codeblock(copyInput) {
	move	$ref(input#1),a		; copy input to output
	move	a,$ref(ouput)
	}

	// Two-input cases

	codeblock(twoInputAnd) {
	move	$ref(input#1),x0		; x0 = input#1
	move	$ref(input#2),x1		; x1 = input#2
	mpy	x1,x0,a		#255,y0	; a = x0 * x1 and y0 = TRUE
	tst	a			; test a
	tne	y0,a			; if (a != 0) a = TRUE
	}

	// General cases

	codeblock(prepareAndLoop) {
	move	#<$addr(input#1)+1,r0		; r0 = input block address + 1
	clr	b	$ref(input#1),a		; a = input#1
	move    b,y0			; y0 = FALSE
	tst	a	x:(r0)+,b
	}

	codeblock(logicAndOpAndLoad,"int numinputs") {
	do	#@numinputs-1,$label(AndLoop)
	teq	y0,a			; if previous = 0, then a = FALSE
	tst	b	x:(r0)+,b	; test previous and load next input
$label(AndLoop)
	teq	y0,a			; if previous = 0, then a = FALSE
	}

	codeblock(logicOrOp) {
	or	x0,a
	}

	codeblock(logicOrOpAndLoad,"int i") {
	or	x0,a	$ref(input#@i),x0
	}

	codeblock(beginXor) {
	move	#<$addr(input#1)+1,r0		; r0 = pointer into input block
	clr	a	$ref(input#1),b		; a contains result
	}

	codeblock(logicXorOpAndLoad,"int numinputs") {
	do	#@numinputs-1,$label(Xor)
	not	a	a,y0		; save a in y0 and invert a
	tst	b	x:(r0)+,b	; test input and read next input
	teq	y0,a			; restore a if input != 0
$label(Xor)
	}

	codeblock(endXor) {
	not	a	a,y0		; save a in y0 and invert a
	tst	b			; test input and read next input
	teq	y0,a			; restore a if input != 0
	}

	// Reusable code blocks

	codeblock(loadAccumulator,"int i") {
	move	$ref(input#@i),a			; read input#@i to accumulator a
	}

	codeblock(loadx0,"int i") {
	move	$ref(input#@i),x0			; read input#@i to register x0
	}

        codeblock(invertAccumulator) {
	clr	b			; b = FALSE
	tst	a	#255,x0		; x0 = TRUE
	teq	x0,a			; if (a == FALSE) a = x0
	tne	b,a			; if (a != FALSE) a = FALSE
	}

        codeblock(saveResult) {
	move    a,$ref(output)
	}

	go {
		int numinputs = input.numberPorts();
		const char* cn = logic;
		StringList header = "; Boolean ";
		header << cn << " for " << input.numberPorts() << " inputs";

		// The inverter (not) star is the simplest case
		if ( numinputs == 1 ) {
			addCode(header);
			if ( test == NOTID || test == NANDID ||
			     test == NORID || test == XNORID ) {
				addCode(inverter);
			}
			else {
				addCode(copyInput);
			}
			return;
		}

		// For the other stars, we loop through the inputs
		int i = 1;
		header << " using ";

		switch( test ) {
		    case ANDID:
		    case NANDID:
			header << "integer multiplication";
			addCode(header);
			if ( numinputs == 2 ) {
				addCode(twoInputAnd);
			}
			else {
				addCode(prepareAndLoop);
				addCode(logicAndOpAndLoad(numinputs));
			}
			break;
		    case ORID:
		    case NORID:
			header << "positive logic";
			addCode(header);
			addCode(loadAccumulator(1));
			addCode(loadx0(2));
			for (i = 3; i < numinputs; i++ ) {
				addCode(logicOrOpAndLoad(i));
			}
			addCode(logicOrOp);
			break;
		    case XORID:
		    case XNORID:
			header << "positive logic";
			addCode(header);
			addCode(beginXor);
			addCode(logicXorOpAndLoad(numinputs));
			addCode(endXor);
			break;
		}

		// Compute final result, adjusting for positive/negative logic
		if (test == NANDID || test == NORID || test == XNORID ) {
			addCode(invertAccumulator);
		}
		addCode(saveResult);
	}
	exectime {
		int numinputs = input.numberPorts();

		// Time to read one input and write one output
		int pairsOfCycles = 2;

		switch( pairsOfCycles ) {
		  case NOTID:
		    pairsOfCycles += 2;
		    break;

		  case NANDID:
		    pairsOfCycles += 4;
		    // fall through
		  case ANDID:
		    if (numinputs == 2) pairsOfCycles += 5;
		    else if (pairsOfCycles > 2) pairsOfCycles += 2*numinputs+2;
		    break;

		  case NORID:
		    pairsOfCycles += 4;
		    // fall through
		  case ORID:
		    pairsOfCycles += numinputs;
		    break;

		  case XNORID:
		    pairsOfCycles += 4;
		    // fall through
		  case XORID:
		    pairsOfCycles += 3*numinputs;
		    break;
		}

		return pairsOfCycles;
	}
}
