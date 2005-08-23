defstar {
	name { Test }
	domain { SDF }
	version { @(#)SDFTest.pl	1.10	10/07/96 }
	author { Rolando Diesta and Edward A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	desc {
This star compares its two inputs "upper" and "lower" using the
comparison test "condition".  The "condition" test can be any one of
{EQ NE GT GE}, or equivalently any one of {== != &gt; &gt;=}, whose
elements represent the binary operations of equals, not equals,
greater than, and greater than or equals, respectively.  If the
"crossingsOnly" parameter is TRUE, then the output is TRUE only when
the outcome of the test changes from TRUE to FALSE or FALSE to TRUE.
In this case, the first output is always TRUE.  This star outputs 0 on
FALSE and 1 on TRUE.
	}
	htmldoc {
To implement the tests "&lt;" or "&lt;=", simply reverse the inputs.
	}
	input {
		name { upper }
		type { float }
		desc { Left-hand side of the test }
	}
	input {
		name { lower }
		type { float }
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
If TRUE, output is TRUE only when the sense of the test changes
		}
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
		// prevResult can be initialized to any value
		prevResult = TRUE;

		const char* cn = condition;
		if ( strcasecmp ( cn, "EQ") == 0) test = EQID;
		else if ( strcasecmp ( cn, "NE") == 0) test = NEID;
		else if ( strcasecmp ( cn, "GT") == 0) test = GTID;
		else if ( strcasecmp ( cn, "GE") == 0) test = GEID;
		else if ( strcasecmp ( cn, "==") == 0) test = EQID;
		else if ( strcasecmp ( cn, "!=") == 0) test = NEID;
		else if ( strcasecmp ( cn, ">") == 0) test = GTID;
		else if ( strcasecmp ( cn, ">=") == 0) test = GEID;
		else Error::abortRun(*this, "Unrecognized test.", cn);
	}
	go {
		int result = FALSE;
		double left = double(upper%0);
		double right = double(lower%0);

		// compare the two current input values
		switch(test) {
			case EQID:  result = (left == right);  break;
			case NEID:  result = (left != right);  break;
			case GTID:  result = (left >  right);  break;
			case GEID:  result = (left >= right);  break;
		}

		// C returns 0 for FALSE and non-zero for TRUE: make it
		// 0 and 1 so that prevResult = -1 works for crossingsOnly
		result = result ? 1 : 0 ;

		if (int(crossingsOnly)) {
			if (prevResult != result)
				output%0 << 1;
			else
				output%0 << 0;
		}
		else
			output%0 << result;

		prevResult = result;
	}
}
