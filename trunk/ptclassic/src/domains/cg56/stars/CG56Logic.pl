defstar {
	name { Logic }
	domain { CG56 }
	version { $Id$ }
	author { Brian L. Evans }
	acknowledge { Edward A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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
	explanation {
The NOT operation requires that there be only one input.
The XOR operation with multiple inputs tests for an odd number
of TRUE values among the inputs.
The other operations are self-explanatory.
.ir "logic"
.ir "Boolean logic"
.ir "not (logical operation)"
.ir "and (logical operation)"
.ir "exclusive or"
.ir "xor (logical operation)"
.ir "xnor (logical operation)"
.ir "nand (logical operation)"
.ir "or (logical operation)"
.ir "nor (logical operation)"
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

	codeblock(loadAccumulator,"int i") {
		move	$ref(input#@i),a	a,x0	; read input, save a
	}

        codeblock(saveStatus) {
		movec   sr,a    ; save status register (has condition codes)
	}

	codeblock(invert) {
		and	#0x04,a		; test Z bit (returns true if zero)
	}

	codeblock(branchIfZero) {
		jeq	$label(_logic_end)	; branch if input is zero
	}

	codeblock(endAnd) {
		move	#1,a		; return TRUE
$label(_logic_end)
	}

	codeblock(logicOrOp) {
		or	x0,a
	}

	codeblock(logicOrOpAndLoad,"int i") {
		or	x0,a	$ref(input#@i),x0
	}

        codeblock(saveResult) {
		move    a,$ref(output)
	}

	go {
		const char* cn = logic;
		StringList header = "; Boolean ";
		header << cn << " for " << input.numberPorts() << " inputs";
		addCode(header);

		// The inverter (not) star is the simplest case
		if ( input.numberPorts() == 1 ) {
			addCode(loadAccumulator(1));
			if ( test == NOTID || test == NANDID ||
			     test == NORID || test == XNORID ) {
				addCode(saveStatus);
				addCode(invert);
			}
			addCode(saveResult);
			return;
		}

		// For the other stars, we loop through the inputs
		int i = 1;
		StringList implementationComment = "; Boolean ";
		implementationComment << cn << " operation using ";

		switch( test ) {
		    case ANDID:
		    case NANDID:
			implementationComment << "positive logic";
			addCode(implementationComment);
			for (i = 1; i < input.numberPorts(); i++ ) {
				addCode(loadAccumulator(i));
				addCode(branchIfZero);
			}
			addCode(endAnd);
			break;
		    case ORID:
		    case NORID:
			implementationComment << "positive logic";
			addCode(implementationComment);
			addCode(loadAccumulator(1));
			addCode(loadAccumulator(2));
			for (i = 2; i < input.numberPorts(); i++ ) {
				addCode(logicOrOpAndLoad(i));
			}
			addCode(logicOrOp);
			break;
		    case XORID:
		    case XNORID:
			implementationComment << "negative logic";
			addCode(implementationComment);
			addCode(loadAccumulator(1));
			addCode(saveStatus);
			addCode(invert);
			for (i = 2; i < input.numberPorts(); i++ ) {
				addCode(loadAccumulator(i));
				addCode(saveStatus);
				addCode(invert);
				addCode(logicOrOp);
			}
			break;
		}

		// Compute final result, adjusting for positive/negative logic
		if (test == NANDID || test == NORID || test == XORID ) {
			addCode(saveStatus);
			addCode(invert);
		}
		addCode(saveResult);
	}
	exectime {
		// Time to read the input and write the output
		int test = 4;

		switch( test ) {
		  case NOTID:
		    test += 4;
		    break;

		  case ANDID:
		  case NANDID:
		    test += 4 + 6*input.numberPorts();
		    break;

		  case ORID:
		  case NORID:
		    test += 2 + 2*input.numberPorts();
		    break;

		  case XORID:
		  case XNORID:
		    test += 4 + 8*input.numberPorts();
		    break;
		}

		return test;
	}
}
