defstar {
	name { TestLevel }
	domain { DE }
	version { $Id$ }
	author { Soonhoi Ha and Edward Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
This star detects threshold crossings if "crossingsOnly" is TRUE.
Otherwise, it simply compares the input against the "threshold".

If "crossingsOnly" is TRUE, then: 
(1) A TRUE is sent to "output" when the "input" particle exceeds or
    equals the "threshold" value, having been previously smaller.
(2) A FALSE is sent when the "input" particle is smaller than
    "threshold" having been previously larger.
Otherwise, no output is produced.

If "crossingsOnly" is FALSE, then a TRUE is sent to "output"
whenever any "input" particle greater than or equal to "threshold"
is received, and a FALSE is sent otherwise.
	}
	explanation {
When "crossingsOnly" is TRUE,
this star internally remembers its previous outputs so that successive
identical outputs are not produced.  The output will always be an
alternating TRUE/FALSE stream.  In this case,
The first output will always be TRUE, and will occur the first time
the input exceeds the threshold.
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
