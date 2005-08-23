defstar {
	name { Test }
	domain { CG56 }
	version { @(#)CG56Test.pl	1.6	01 Oct 1996 }
	author { Rolando Diesta, Brian L. Evans, and Edward A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
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
	// The A_YMEM attribute is set or cleared below in the setup method
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
			prevResult.setAttributes(A_YMEM);
		}
		else {
			// FIXME: need to declare no internal state
			prevResult.clearAttributes(A_YMEM);
		}
	}

	// Register usage:
	//   accumulator a   initially upper input; at end, output of test
	//   accumulator b   zero
	//   register x0     lower input
	//   register y0     previous result

	// 1. Load inputs

	codeblock(data) {
; Test star: a = upper input and x0 = lower input
	clr	b	$ref(lower),x0
	move	$ref(upper),a
	}

	codeblock(dataAndPrevResult) {
; Test star: a = upper input, x0 = lower input, and y0 = previous result
	clr	b	$ref(lower),x0	$ref(prevResult),y0
	move	$ref(upper),a
	}

	// 2. Perform comparison: output 0 for FALSE and 1 for TRUE

	codeblock(equalTest) {
	cmp	x0,a	#$$01,a		; test a - x0 and set a = TRUE
	tne	b,a			; if lower != upper then a = FALSE
	}
	codeblock(notEqualTest) {
	sub	x0,a			; test a - x0 which returns a Boolean
	}
	codeblock(greaterThan) {
	cmp	x0,a	#$$01,a		; test a - x0 and set a = TRUE
	tle	b,a			; if upper <= lower, then a = FALSE
	}
	codeblock(greaterEqual) {
	cmp	x0,a	#$$01,a		; test a - x0 and set a = TRUE
	tlt	b,a			; if upper < lower, then a = FALSE
	}

	// 3. Post-process

	codeblock(crossings) {
; compare previous and current results, and save current result
	sub	y0,a	a,$ref(prevResult)
	}

	// 4. Save result
	codeblock(saveResult) {
	move	a,$ref(output)
	}

	go {
		StringList header = "; Testing if upper ";
		header << (const char*)condition << " lower\n";
		addCode(header);

		if ( int(crossingsOnly) ) addCode(dataAndPrevResult);
		else addCode(data);

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
		}

		addCode(saveResult);
	}
	exectime {
		// Time to read in the two inputs and write out one result
		int pairsOfCycles = 3;

		// Add number of pairs of cycles
		if ( int(crossingsOnly) ) pairsOfCycles++;
		switch (test) {
		    case EQID:
			pairsOfCycles += 2;
			break;
		    case NEID:
			pairsOfCycles += 1;
			break;
		    case GTID:
			pairsOfCycles += 2;
			break;
		    case GEID:
			pairsOfCycles += 2;
			break;
		}
		return pairsOfCycles;
	}
}
