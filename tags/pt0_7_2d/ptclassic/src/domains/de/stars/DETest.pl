defstar {
	name { Test }
	domain { DE }
	version { @(#)DETest.pl	1.9	10/07/96 }
	author { Rolando Diesta, Edward A. Lee, and Brian L. Evans }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE logic library }
	desc {
This star compares its two inputs "upper" and "lower" using the
comparison test "condition".  The "condition" test can be any of {EQ
NE LT LE GT GE} or equivalently any one of {== != &lt; &lt;= &gt;
&lt;=}, whose elements represent the binary operations of equals, not
equals, less-than, less-than or equals, etc.  If "crossingsOnly"
parameter is TRUE, then an output event is generated only when the
value of the output changes.  Hence, the output events will always
alternate between TRUE and FALSE.  This star outputs 0 on FALSE and 1
on TRUE.
	}
	htmldoc {
Like all DE stars, this star will fire when it gets an event on either input.
If no event has arrived on the other input, then the value of the other input
is assumed to be zero.
<p>
Even if "crossingsOnly" is TRUE, the first test result will always be sent
to the output.  That first test result occurs when the first event arrives
on either input.
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
		desc { Indicates the result of the test }
	}
	defstate {
		name { condition }
		type { string }
		default { "EQ" }
		desc {
The test condition should be one of EQ NE LT LE GT or GE
		}
	}
	defstate {
		name { crossingsOnly }
		type { int }
		default { "FALSE" }
		desc {
If TRUE, the output is triggered only when the sense of the test changes
		}
	}
	code {
#define EQID 0
#define NEID 1
#define LTID 2
#define LEID 3
#define GTID 4
#define GEID 5
	}
	private {
		int prevResult;
		int test;
		double left;
		double right;
	}
	setup {
		// Arbitrarily set the current inputs to zero
		left = right = 0.0;

		// Set prevResult so crossingsOnly works right for first input
		prevResult = -1;

		// Determine which test the user has chosen
		const char* cn = condition;
		if ( strcasecmp ( cn, "EQ") == 0) test = EQID;
		else if ( strcasecmp ( cn, "NE") == 0) test = NEID;
		else if ( strcasecmp ( cn, "LT") == 0) test = LTID;
		else if ( strcasecmp ( cn, "LE") == 0) test = LEID;
		else if ( strcasecmp ( cn, "GT") == 0) test = GTID;
		else if ( strcasecmp ( cn, "GE") == 0) test = GEID;
		else if ( strcasecmp ( cn, "==") == 0) test = EQID;
		else if ( strcasecmp ( cn, "!=") == 0) test = NEID;
		else if ( strcasecmp ( cn, "<") == 0) test = LTID;
		else if ( strcasecmp ( cn, "<=") == 0) test = LEID;
		else if ( strcasecmp ( cn, ">") == 0) test = GTID;
		else if ( strcasecmp ( cn, ">=") == 0) test = GEID;
		else Error::abortRun(*this, "Unrecognized test: ", cn);
	}

	go {
		int result = FALSE;

		completionTime = arrivalTime;

		// update the two current input values
		if (upper.dataNew) left = double(upper%0);
		if (lower.dataNew) right = double(lower%0);

		// compare the current two input values
		switch(test) {
			case EQID:  result = (left == right);  break;
			case NEID:  result = (left != right);  break;
			case LTID:  result = (left <  right);  break;
			case LEID:  result = (left <= right);  break;
			case GTID:  result = (left >  right);  break;
			case GEID:  result = (left >= right);  break;
		}

		// C gives 0 for FALSE and non-zero for TRUE; make it
		// 0 and 1 so that prevResult = -1 works for crossingsOnly
		result = result ? 1 : 0 ;

		// output the current result unless crossingsOnly state is
		// true and the current result is different from the previous
		if ( int(crossingsOnly) ) {
			if (prevResult != result) {
				output.put(completionTime) << result;
			}
		}
		else {
			output.put(completionTime) << result;
		}

		prevResult = result;
	}
}
