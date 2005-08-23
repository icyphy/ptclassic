defstar {
	name { Logic }
	domain { SDF }
	version { @(#)SDFLogic.pl	1.11	01 Oct 1996 }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
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
		else Error::abortRun(*this, "Unrecognized test ", cn );
	}
	go {
	    MPHIter nexti(input);
	    PortHole* p = 0;
	    int result = FALSE;

	    switch( test ) {
		case NOTID:
		    p = nexti++;
		    result = ! int((*p)%0);
		    break;
		case ANDID:
		case NANDID:
		    result = TRUE;
		    while ((p = nexti++) != 0)
			result = result && int((*p)%0);
		    if (test == NANDID) result = !result;
		    break;
		case ORID:
		case NORID:
		    result = FALSE;
		    while ((p = nexti++) != 0)
			result = result || int((*p)%0);
		    if (test == NORID) result = !result;
		    break;
		case XORID:
		case XNORID:
		    result = FALSE;
		    while ((p = nexti++) != 0)
			if (int((*p)%0)) result = !result;
		    if (test == XNORID) result = !result;
		    break;
	    }
	    output%0 << result;
	}
}
