defstar {
	name { TestLevel }
	domain { DE }
	version { $Id$}
	author { Soonhoi Ha and Edward Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
When "crossingsOnly" is TRUE, this star internally remembers its
previous outputs so that successive identical outputs are not produced.
The output will always be an alternating TRUE/FALSE stream.  In this case,
the first output will always be TRUE, and will occur the first time
the input exceeds the threshold.  When "crossingsOnly" is FALSE,
then this star outputs TRUE if the input exceeds the "threshold"
and FALSE otherwise.  Output values are 1 for TRUE and 0 for FALSE.
	}
	explanation {
This star detects threshold crossings if \fIcrossingsOnly\fR is TRUE.
Otherwise, it simply compares the input against the "threshold".
.lp
If "crossingsOnly" is TRUE, then: 
.ip 1)
A TRUE is output when the input particle exceeds or
equals the \fIthreshold\fR value, having been previously smaller.
.ip 2)
A FALSE is output when the input particle is smaller than
\fIthreshold\fR having been previously larger.
.lp
Otherwise, no output is produced.
.pp
If \fIcrossingsOnly\fR is FALSE, then a TRUE is output whenever any
input particle greater than or equal to \fIthreshold\fR is received,
and a FALSE is sent otherwise.
Output values are always 1 for TRUE and 0 for FALSE.
	}
	input {
		name { input }
		type { float }
		desc { Value to be compared against the threshold. }
	}
	output {
		name { output }
		type { int }
		desc { Indicates level crossings or result of compare.}
	}
	defstate {
		name { threshold }
		type { float }
		default { "10" }
		desc { The level for comparison against input particles. }
	}
	defstate {
		name { crossingsOnly }
		type { int }
		default { "TRUE" }
		desc { If TRUE, outputs occur only on level crossings. }
	}
	private {
		int flag;
	}
	code {
#define TRUE 1
#define FALSE 0
	}
	setup {
		flag = FALSE;
	}

	go {
		completionTime = arrivalTime;

		// compare the input value with the "threshold"
		if (double(input%0) < double(threshold)) {
		   if (int(crossingsOnly)) {
			if (flag == TRUE) {
				flag = FALSE;
				// Indicate downward level crossing
				output.put(completionTime) << flag;
			} 
		   } else output.put(completionTime) << FALSE;
		} else {
		   if (int(crossingsOnly)) {
			if (flag == FALSE) {
				flag = TRUE;
				// send output
				output.put(completionTime) << flag;
			}
		   } else output.put(completionTime) << TRUE;
		}
	}
}
