defstar {
	name { Logic }
	domain { SDF }
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF logic library }
	desc {
This star applies a logical operation to any number of inputs.
The inputs are integers interpreted as Booleans,
where zero is a FALSE and nonzero is a TRUE.
The logical operations supported are {NOT AND NAND OR NOR XOR XNOR}.
	}
	explanation {
The NOT operation requires that there be only one input.
The XOR operation with multiple inputs tests for an odd number
of TRUEs among the inputs.  The other operations are self explanatory.
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
		desc { Input logic values }
	}
	output {
		name { output }
		type { int }
		desc { Result of the logic test }
	}
	defstate {
		name { logic }
		type { string }
		default { "AND" }
		desc { The test logic: one of NOT AND NAND OR NOR XOR or XNOR }
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
		if ( strcasecmp ( cn, "NOT") == 0) {
		    test = NOTID;
		    if (input.numberPorts() > 1)
			Error::abortRun(*this,
			    "NOT operation can only have one input");
		}
		else if ( strcasecmp ( cn, "AND") == 0) test = ANDID;
		else if ( strcasecmp ( cn, "NAND") == 0) test = NANDID;
		else if ( strcasecmp ( cn, "OR") == 0) test = ORID;
		else if ( strcasecmp ( cn, "NOR") == 0) test = NORID;
		else if ( strcasecmp ( cn, "XOR") == 0) test = XORID;
		else if ( strcasecmp ( cn, "XNOR") == 0) test = XNORID;
		else Error::abortRun(*this,"Unrecognized test.");
	}
	go {
	    MPHIter nexti(input);
	    PortHole *p;
	    int result;

	    switch( test ) {
		case NOTID:
		    p = nexti++;
		    result = !int((*p)%0);
		    break;
		case ANDID:
		case NANDID:
		    result = 1;
		    while ((p = nexti++) != 0)
			result = result && int((*p)%0);
		    if (test == NANDID) result = !result;
		    break;
		case ORID:
		case NORID:
		    result = 0;
		    while ((p = nexti++) != 0)
			result = result || int((*p)%0);
		    if (test == NORID) result = !result;
		    break;
		case XORID:
		case XNORID:
		    result = 0;
		    while ((p = nexti++) != 0)
			if (int((*p)%0)) result = !result;
		    if (test == XNORID) result = !result;
		    break;
	    }
	    output%0 << result;
	}
}
