defstar {
	name { Test }
	domain { C50 }
	version { @(#)C50Test.pl	1.4  01 Oct 1996 }
	author { Luis Gutierrez }
	acknowledge { Rolando Diesta, Brian L. Evans, and Edward A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	desc {
This star compares its two inputs "upper" and "lower" using the comparison
test "condition".  The "condition" test can be any one of {EQ NE GT GE},
or equivalently any one of {== != > >=}, whose elements represent the binary
operations of equals, not equals, greater than, and greater than or equals,
respectively.  If the "crossingsOnly" parameter is TRUE, then the output is
TRUE only when the outcome of the test changes from TRUE to FALSE or FALSE
to TRUE.  In this case, the first output is always TRUE.  This star outputs
0 on FALSE and 1 on TRUE.
	}
	htmldoc {
To implement the tests "&lt;" or "&lt;=", simply reverse the inputs.
	}
	input {
		name { upper }
		type { fix }
		desc { Left-hand side of the test }
	}
	input {
		name { lower }
		type { fix }
		desc { Right-hand side of the test }
	}
	output {
		name { output }
		type { int }
		desc { Result of the test }
	}
	defstate {
		name { condition }
		type { string }
		default { "EQ" }
		desc { The test condition, i.e, one of EQ, NE, LT, or LE }
	}
	defstate {
		name { crossingsOnly }
		type { int }
		default { "FALSE" }
		desc {
If TRUE, outputs are TRUE only when the sense of the test changes
		}
	}
	// The A_UMEM attribute is set or cleared below in the setup method
	defstate {
		name { prevResult }
		type { int }
		default { "-1" }
		desc {
Initial value indicates that prevResult has not yet gotten the result of a
test.  This ensures that the first test result will always be TRUE.
		}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	private {
		int test;
	}
	code {
#define EQID 0
#define NEID 1
#define GTID 2
#define GEID 3
	}
        constructor {
		test = EQID;
	}
	setup {
		const char* cn = condition;
		if ( strcasecmp(cn, "EQ") == 0) test = EQID;
		else if ( strcasecmp(cn, "NE") == 0 ) test = NEID;
		else if ( strcasecmp(cn, "GT") == 0 ) test = GTID;
		else if ( strcasecmp(cn, "GE") == 0 ) test = GEID;
		else if ( strcasecmp(cn, "==") == 0 ) test = EQID;
		else if ( strcasecmp(cn, "!=") == 0 ) test = NEID;
		else if ( strcasecmp(cn, ">") == 0 ) test = GTID;
		else if ( strcasecmp(cn, ">=") == 0 ) test = GEID;
		else Error::abortRun(*this, "Unrecognized test.", cn);

		// This state has radically different behavior
		// depending on the setting of crossingsOnly
		if ( int(crossingsOnly) ) {
			// FIXME: need to declare internal state
			prevResult.setAttributes(A_UMEM);
		}
		else {
			// FIXME: need to declare no internal state
			prevResult.clearAttributes(A_UMEM);
		}
	}


	codeblock(data) {
	setc	ovm
	lar	ar0,#$addr(lower)
	lar	ar1,#$addr(upper)
	mar	*,ar1
	lacc	*,16,ar0	; accH = upper
	sub	*,16,ar2	; acc = upper - lower
	}

	codeblock(equalTest) {
	lar	ar2,#0000h
	xc	1,eq
	adrk	#01h
	}
	codeblock(notEqualTest) {
	lar	ar2,#0000h
	xc	1,neq
	adrk	#01h
	}
	codeblock(greaterThan) {
	lar	ar2,#0000h
	xc	1,gt
	adrk	#01h
	}
	codeblock(greaterEqual) {
	lar	ar2,#0001
	xc	1,lt
	sbrk	#01h
	}


	codeblock(crossings) {
; compare previous and current results, and save current result
	lar	ar3,#$addr(output)
	lar	ar4,#$addr(prevResult)
	mar	*,ar4
	lamm	ar2		; acc = ar2 = {1, 0 }
	xor	*		; acc = ar2 XOR prevResult	
	sar	ar2,*,ar3	; prevResult = ar2
	xc	1,neq
	lacl	#01h		; if(prevResult XOR result) ? acc = 1:acc = 0
	sacl	*		
	clrc	ovm
	}

	codeblock(noCrossings) {
	smmr	ar2,#$addr(output)
	clrc	ovm
	}

	go {
		StringList header = "; Testing if upper ";
		header << (const char*)condition << " lower\n";
		addCode(header);

		addCode(data);

		// Evaluate test, leaving the result in accumulator a
		switch (test) {
		    case EQID:
			addCode(equalTest);
			break;
		    case NEID:
			addCode(notEqualTest);
			break;
		    case GTID:
			addCode(greaterThan);
			break;
		    case GEID:
			addCode(greaterEqual);
			break;
		}

		if ( int(crossingsOnly) ) {
		    addCode(crossings);
		} else {
		    addCode(noCrossings);
		}

	}
	exectime {

		int instructions = 10;

		// Add number of instructions
		if ( int(crossingsOnly) ) instructions += 7;

		return instructions;
	}
}






