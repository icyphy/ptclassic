defstar {
	name { Test }
	domain { SDF }
	version { $Id$ }
	author { Rolando Diesta and Edward A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF logic library }
	desc {
This star compares two inputs.  The test condition can be any of
{EQ NE GT GE} or {== != > >=}, resulting in equals,
not equals, greater-than, or greater-than or equals.

If "crossingsOnly" is TRUE, then the output is TRUE only
when the outcome of the test changes from TRUE to FALSE
or FALSE to TRUE.  In this case, the first output is always
TRUE.
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
		desc { Left hand side of the test. }
	}
	input {
		name { lower }
		type { float }
		desc { Right hand side of the test. }
	}
	output {
		name { output }
		type { int }
		desc { Indicates the result of the test.}
	}
	defstate {
		name { condition }
		type { string }
		default { "EQ" }
		desc { The test condition: one of EQ NE LT or LE }
	}
	defstate {
		name { crossingsOnly }
		type { int }
		default { "FALSE" }
		desc { If TRUE, outputs are TRUE only when the sense of the test changes. }
	}
	code {
#define EQID 0
#define NEID 1
#define GTID 2
#define GEID 3
	}
	private {
		int prevResult;
		int test;
	}
	setup {
		// Initial value indicates that prevResult has not yet
		// gotten the result of a test.
		// This ensures that the first test result will
		// always be TRUE.
		prevResult = -1;

		const char* cn = condition;
		if ( strcasecmp ( cn, "EQ") == 0) test = EQID;
		else if ( strcasecmp ( cn, "NE") == 0) test = NEID;
		else if ( strcasecmp ( cn, "GT") == 0) test = GTID;
		else if ( strcasecmp ( cn, "GE") == 0) test = GEID;
		else if ( strcasecmp ( cn, "==") == 0) test = EQID;
		else if ( strcasecmp ( cn, "!=") == 0) test = NEID;
		else if ( strcasecmp ( cn, ">") == 0) test = GTID;
		else if ( strcasecmp ( cn, ">=") == 0) test = GEID;
		else Error::abortRun(*this,"Unrecognized test.");
	}
	go {
		int result;
		double left = double(upper%0);
		double right = double(lower%0);

		// compare the values
		switch( test ) {
			case EQID:  result  =  left == right;  break;
			case NEID:  result  =  left != right;  break;
			case GTID:  result  =  left >  right;  break;
			case GEID:  result  =  left >= right;  break;
		}
		if (int(crossingsOnly)) {
			if (prevResult != result)
				output%0 << 1;
			else
				output%0 << 0;
		} else output%0 << result;
		prevResult = result;
	}
}
