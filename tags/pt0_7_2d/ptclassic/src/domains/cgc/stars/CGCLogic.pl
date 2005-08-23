defstar {
	name { Logic }
	domain { CGC }
	version { @(#)CGCLogic.pl	1.5	01 Oct 1996 }
	author { Brian L. Evans and Edward A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
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

	codeblock(logicAndOp,"int i") {
	result = result && $ref(input#@i);
	}

	codeblock(logicOrOp,"int i") {
	result = result || $ref(input#@i);
	}

	codeblock(logicXorOp,"int i") {
	if ( $ref(input#@i) ) result = ! result;
	}

	go {
		// The inverter (not) star is the simplest case
		if ( test == NOTID ) {
			addCode("\t$ref(output) = ! $ref(input#1);\n");
			return;
		}

		// Declare and initialize local variables i and result
		if ( test == ANDID || test == NANDID ) {
			addCode("\tint result = 1;\n");
		}
		else {
			addCode("\tint result = 0;\n");
		}

		// Generate the code that walks through the input values
		int i = 1;
		switch( test ) {
		  case ANDID:
		  case NANDID:
			for (i = 1; i <= input.numberPorts(); i++ ) {
			    addCode(logicAndOp(i));
			}
			break;
		  case ORID:
		  case NORID:
			for (i = 1; i <= input.numberPorts(); i++ ) {
			    addCode(logicOrOp(i));
			}
			break;
		  case XORID:
		  case XNORID:
			for (i = 1; i <= input.numberPorts(); i++ ) {
			    addCode(logicXorOp(i));
			    break;
			}
		}

		// Compute final result 
		StringList finalCode;
		if (test == NANDID || test == NORID || test == XNORID ) {
			finalCode << "\tresult = ! result;\n";
		}
		finalCode << "\t$ref(output) = result;\n";
		addCode(finalCode);
	}
}
