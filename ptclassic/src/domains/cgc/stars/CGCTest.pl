defstar {
	name { Test }
	domain { CGC }
	version { $Id$ }
	author { Rolando Diesta, Brian L. Evans, and Edward A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
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
		type { float }
		desc { Left-hand side of the test. }
	}
	input {
		name { lower }
		type { float }
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
		desc { If TRUE, outputs are TRUE only when the sense of the test changes. }
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
	defstate {
		name { test }
		type { int }
		default { "-1" }
		desc { Logic test }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	code {
#define EQID 0
#define NEID 1
#define GTID 2
#define GEID 3
	}
	setup {
		const char* cn = condition;
		if ( strcasecmp(cn, "EQ") == 0) test = EQID;
		else if ( strcasecmp(cn, "NE") == 0) test = NEID;
		else if ( strcasecmp(cn, "GT") == 0) test = GTID;
		else if ( strcasecmp(cn, "GE") == 0) test = GEID;
		else if ( strcasecmp(cn, "==") == 0) test = EQID;
		else if ( strcasecmp(cn, "!=") == 0) test = NEID;
		else if ( strcasecmp(cn, ">") == 0) test = GTID;
		else if ( strcasecmp(cn, ">=") == 0) test = GEID;
		else Error::abortRun(*this, "Unrecognized test.");
	}
	codeblock(macros) {
#define EQID 0
#define NEID 1
#define GTID 2
#define GEID 3
	}
	codeblock(decl) {
		int result = 0;
		double left = $ref(upper,0);
		double right = $ref(lower,0);
	}
	codeblock(compare) {
		/* compare the values */
		switch ( $val(test) ) {
			case EQID:  result = (left == right);  break;
			case NEID:  result = (left != right);  break;
			case GTID:  result = (left >  right);  break;
			case GEID:  result = (left >= right);  break;
		}
		if ( $val(crossingsOnly) ) {
			if ( $ref(prevResult) != result )
				$ref(output,0) = 1;
			else
				$ref(output,0) = 0;
		}
		else
			$ref(output,0) = result;
		$ref(prevResult) = result;
	}
	go {
		addCode(macros);
		addCode(decl);
		addCode(compare);
	}
}
