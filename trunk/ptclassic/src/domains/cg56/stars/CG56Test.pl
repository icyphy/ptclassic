defstar {
	name { Test }
	domain { CG56 }
	version { $Id$ }
	author { Rolando Diesta, Brian L. Evans, and Edward A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	desc {
This star compares its two inputs "upper" and "lower".
The test "condition" can be any one of {EQ NE GT GE},
or equivalently any one of {== != > >=},
whose elements represent the binary operations of equals, not equals,
greater than, and greater than or equals, respectively.

If the "crossingsOnly" parameter is TRUE, then the output is TRUE only
when the outcome of the test changes from TRUE to FALSE
or FALSE to TRUE.
In this case, the first output is always TRUE.
	}
	explanation {
To implement the tests "<" or "<=", simply reverse the inputs.
.ir "comparison"
.ir "Boolean test"
.ir "equals"
.ir "not equals"
.ir "less than"
.ir "less than or equals"
.ir "greater than"
.ir "greater than or equals"
	}
	input {
		name { upper }
		type { fix }
		desc { Left-hand side of the test. }
	}
	input {
		name { lower }
		type { fix }
		desc { Right-hand side of the test. }
	}
	output {
		name { output }
		type { int }
		desc { Result of the test. }
	}
	defstate {
		name { condition }
		type { string }
		default { "EQ" }
		desc { The test condition, i.e, one of EQ, NE, LT, or LE. }
	}
	defstate {
		name { crossingsOnly }
		type { int }
		default { "FALSE" }
		desc {
If TRUE, outputs are TRUE only when the sense of the test changes.
		}
	}
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
		else Error::abortRun(*this, "Unrecognized test.");

		// This state has radically different behavior
		// depending on the setting of crossingsOnly
		if ( int(crossingsOnly) ) {
			// FIXME: need to declare internal state
			prevResult.setAttributes(A_XMEM);
		}
		else {
			// FIXME: need to declare no internal state
			prevResult.clearAttributes(A_XMEM);
		}
	}
	codeblock(data) {
		move	$ref(lower),x0		; load inputs from X memory
		move	$ref(upper),a
	}
	codeblock(compare) {
		cmp	x0,a
	}
	codeblock(compareCrossings) {
		cmp	x0,a	$ref(prevResult),x1
	}
	codeblock(saveStatus) {
		movec	sr,a	; save status register (has condition codes)
	}
	codeblock(crossings) {
; compare previous and current results, and save current result
		cmp	x1,a	a,$ref(prevResult)
	}
	codeblock(saveResult) {
		move	a,$ref(output)
	}
	go {
		StringList header = "; Testing if $ref(upper) ";
		header << (const char*)condition << " $ref(lower)\n"
		       << "; compare $ref(lower) to $ref(upper) and "
		       << "return masked condition codes\n"
		       << "; using zero (Z) and negative (N) condition bits";
		addCode(header);

		addCode(data);
		if ( int(crossingsOnly) ) addCode(compareCrossings);
		else addCode(compare);

		// Evaluate test, leaving the result in accumulator a
		switch (test) {
		    case EQID:
			addCode(saveStatus);
			addCode("and #0x04,a	; test Z bit");
			break;
		    case NEID:
			addCode("; accumulator is zero when upper = lower");
			addCode("; and non-zero otherwise; so just return it");
			break;
		    case GTID:
			addCode(saveStatus);
			addCode("and #0x0B,a	; test N and Z bits");
			addCode("eor #0x04,a	; make sure Z bit is off");
			break;
		    case GEID:
			addCode(saveStatus);
			addCode("and #0x0B,a	; test N and Z bits");
			break;
		}

		if ( int(crossingsOnly) ) {
		    addCode(crossings);
		}

		addCode(saveResult);
	}
	exectime {
		// Time to read in the two inputs and write out the result
		int ticks = 6;
		// Add number of cycles
		if ( int(crossingsOnly) ) ticks += 2;
		switch (test) {
		    case EQID:
			ticks += 6;
			break;
		    case NEID:
			ticks += 2;
			break;
		    case GTID:
			ticks += 8;
			break;
		    case GEID:
			ticks += 6;
			break;
		}
		return ticks;
	}
}
