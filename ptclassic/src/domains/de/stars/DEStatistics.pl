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
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	explanation {
Calculate the time average and variance of the input values that have
arrived since the last reset.  The execution order of the inputs is
demand->reset->input.  Multiple simultaneous demand/reset inputs are
ignore except one.
.pp
The computation is performed based on the zero-th order interpolation.
When an input data enters, we assume that the data value is hold until
the next input arrives.
	}
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
		double sum;
		double squareSum;
		double previous;
		double startTime;
	}
	constructor {
		input.triggers();
		reset.triggers();
		setMode(PHASE);
	}
	setup {
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
			double leng = arrivalTime - startTime;
			double span = arrivalTime - completionTime;
			sum += previous * span;
			squareSum += previous*previous*span;
			// output
			if (leng) {
			   double t1 = sum/leng;
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
		while (input.dataNew) {
			if (startTime < 0) {
			      startTime = arrivalTime;
			} else {
			   	double span = arrivalTime - completionTime;
			 	sum += previous * span;
			     	squareSum += previous * previous * span;
			}
			previous = input.get();
			// renew the completionTime
			completionTime = arrivalTime;
		}
		// renew the completionTime
		completionTime = arrivalTime;
	}
}

