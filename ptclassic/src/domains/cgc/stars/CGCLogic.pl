defstar {
	name { Logic }
	domain { CGC }
	version { $Id$ }
	author { Brian L. Evans and Edward A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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
	go {
		// The inverter (not) star is the simplest case
		if ( test == NOTID ) {
			addCode("$ref(output) = ! $ref(input,#1);\n");
			return;
		}

		// Declare and initialize local variables i and result
		StringList iterator = "int i = 0;\n";
		if ( test == ANDID || test == NANDID ) {
			iterator << "int result = 1;\n";
		}
		else {
			iterator << "int result = 0;\n";
		}

		// Generate the code that walks through the input values
		iterator << "for (; i < "
			 << input.numberPorts()
			 << "; i++ ) {\n";
		switch( test ) {
		  case ANDID:
		  case NANDID:
		    iterator << "\tresult = result && $ref(input,#@i);\n";
		    break;
		  case ORID:
		  case NORID:
		    iterator << "\tresult = result || $ref(input,#@i);\n";
		    break;
		  case XORID:
		  case XNORID:
		    iterator << "\tif ( $ref(input,#@i) ) {\n"
			     << "\t\tresult = ! result;\n"
			     << "\t}\n";
		    break;
		}

		iterator << "}\n";
		if (test == NANDID || test == NORID || test == XNORID ) {
			iterator << "\tresult = ! result;\n";
		}
		iterator << "$ref(output) = result;\n";
		addCode(iterator);
	}
}
