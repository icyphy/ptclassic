defstar {
	name { Trainer }
	domain { SDF }
	desc {
Pass the value of the "train" input to the output for the first "trainLength" samples,
then pass the "decision" input to the output. This star is designed for use with
adaptive equalizers that require a training sequence at startup, but it can be
used whenever one sequence is used during a startup phase, and another sequence
after that. 
	}
	version { $Id$ }
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
During the startup phase, the "decision" inputs are discarded.  After the
startup phase, the "train" inputs are discarded.
.Id "DFE training"
.Id "decision feedback equalizer training"
.Id "equalizer, decision feedback, training"
	}
	defstate {
		name { trainLength }
		type { int }
		default { 100 }
		desc { Number of training samples to use }
	}
	input {
		name { train }
		type { anytype }
	}
	input {
		name { decision }
		type { =train }
	}
	output {
		name { output }
		type { = train }
	}
	protected {
		int count;
	}
	setup {
		count = 0;
	}
	go {
		if (count < int(trainLength)) {
			output%0 = train%0;
			count++;
		}
		else {
			output%0 = decision%0;
		}
	}
}
