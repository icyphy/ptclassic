ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 11/30/90

 It calculates the average and variance of inputs in time from the 
 last reset time.  
 It generates the average and variance outputs on demand input.
 The start time is given by the reset input. If an input event comes
 before the reset input, the start time is the arrival time of the
 first input.  The reset input restarts the statistics.
 demand input before the start time is ignored.

**************************************************************************/
}
defstar {
	name { Statistics }
	domain { DE }
	desc {	"Calculate the statistics (average, variance) of the \n"
		"input values in time since the last reset. \n" 
		"Output is generated on each demand input.\n"
		"When the reset input comes, reset the content and restarts.\n"
		"When demand and reset arrives at the same time,\n"
		"output first and reset next.\n"
	}
	input {
		name { input }
		type { float }
	}
	input {
		name { reset }
		type { anytype }
	}
	input {
		name { demand }
		type { anytype }
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

