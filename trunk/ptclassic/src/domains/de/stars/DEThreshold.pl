defstar {
	name { Threshold }
	domain { DE }
	version { $Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	desc {
A particle on the "input" is compared to "threshold".
If its value is larger or equal, and was previously smaller, then a TRUE is output.
If its value is smaller, and was previously larger or equal, then a FALSE is output.
Otherwise, no output is produced.
	}
	explanation {
This block internally remembers its previous outputs so that successive
identical outputs are not produced.  The output will always be an
alternating TRUE/FALSE stream.
The first output will always be TRUE, and will occur the first time
the input exceeds the threshold.
	}
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { int }
		desc { Always an alternating TRUE/FALSE sequence. }
	}
	defstate {
		name { threshold }
		type { float }
		default { "10" }
		desc { For comparison against input particles. }
	}
	private {
		int flag;
	}
	start {
		flag = FALSE;
	}

	go {
		completionTime = arrivalTime;

		// compare the input value with the "threshold"
		if (int(input%0) < int(threshold)) {  	// no blockage
			if (flag == TRUE) {
				flag = FALSE;
				// send blockRelease output
				output.put(completionTime) << flag;
			} 
		} else {
			if (flag == FALSE) {
				flag = TRUE;
				// send output
				output.put(completionTime) << flag;
			}
		}
	}
}
