defstar {
	name { Test }
	domain { DE }
	version { $Id$ }
	author { Rolando Diesta and Edward Lee }
	copyright { 1991 The Regents of the University of California }
	location { DE logic library }
	desc {
This star compares two inputs.  The test condition can be any of
{EQ NE LT LE GT GE} or {== != < <= > >=}, resulting in equals,
not equals, less-than, less-than or equals, etc.

If "crossingsOnly" is TRUE, then an output event is generated only
when the value of the output changes.  Hence the output events
will always alternate between true and false.
	}
	explanation {
Like all DE stars, this star will fire when it gets an event on either input.
If no event has arrived on the other input, then the value of the other input
is assumed to be zero.
.pp
Even if "crossingsOnly" is TRUE, the first test result will always be sent
to the output.  That first test result occurs when the first event arrives
on either input.
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
		desc { The test condition: one of EQ NE LT LE GT or GE }
	}
	defstate {
		name { crossingsOnly }
		type { int }
		default { "FALSE" }
		desc { If TRUE, outputs occur only when the sense of the output changes. }
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
		int flag;
		double left;
		double right;
		int test;
	}
	start {
		// Initial value indicates that flag has not yet gotten the result of a test.
		// This ensures that the first test result will always be output.
		flag = -1;
		left = right = 0.0;

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
	}

	go {
		int result=0;

		completionTime = arrivalTime;

		if (upper.dataNew) left = double(upper%0);
		if (lower.dataNew) right = double(lower%0);

		// compare the values
		switch( test ) {
			case EQID:  result  =  left == right;  break;
			case NEID:  result  =  left != right;  break;
			case LTID:  result  =  left <  right;  break;
			case LEID:  result  =  left <= right;  break;
			case GTID:  result  =  left >  right;  break;
			case GEID:  result  =  left >= right;  break;
		}
		if (int(crossingsOnly)) {
			if (flag != result) {
				output.put(completionTime) << result;
			} 
		} else output.put(completionTime) << result;
		flag = result;
	}
}
