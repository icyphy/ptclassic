defstar {
	name { Statistics }
	domain { DE }
	desc {
Calculate the average and variance of the
input values that have arrived since the last reset.
An output is generated when a "demand" input is received.
When a "reset" input arrives, the calculations are restarted.
When "demand" and "reset" particles arrive at the same time,
an output is produced before the calculations are restarted.
	}
	version { $Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	input {
		name { input }
		type { float }
	}
	input {
		name { reset }
		type { anytype }
		desc { Restart the calculations. }
	}
	input {
		name { demand }
		type { anytype }
		desc { Stimulate an output. }
	}
	output {
		name { average }
		type { float }
	}
	output {
		name { variance }
		type { float }
	}
	private {
		float sum;
		float squareSum;
		float previous;
		float startTime;
	}
	start {
		previous = 0;
		sum = 0;
		squareSum = 0;
		startTime = -1.0;
	}
	go {
		// on demand input
		if (demand.dataNew) {
			demand.dataNew = FALSE;
			if (startTime < 0 ) return;
			float leng = arrivalTime - startTime;
			float span = arrivalTime - completionTime;
			sum += previous * span;
			squareSum += previous*previous*span;
			// output
			if (leng) {
			   float t1 = sum/leng;
			   average.put(arrivalTime) << t1;
			   variance.put(arrivalTime) << squareSum/leng - t1*t1;
			}
		}
		// check reset input
		if (reset.dataNew) {
			reset.dataNew = FALSE;
			startTime = arrivalTime;
			sum = 0; squareSum = 0; previous = 0;
		}
		// on normal input
		if (input.dataNew) {
			if (startTime < 0) {
			      startTime = arrivalTime;
			} else {
			   	float span = arrivalTime - completionTime;
			 	sum += previous * span;
			     	squareSum += previous * previous * span;
			}
			previous = float(input.get());
		}
		// renew the completionTime
		completionTime = arrivalTime;
	}
}

