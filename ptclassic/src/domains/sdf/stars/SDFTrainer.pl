defstar {
	name { Trainer }
	domain { SDF }
	desc {
Passes the "train" input to the output for the first "trainLength"
samples, then passes the "decision" input to the output.  Designed
for use in decision feedback equalizers, but can be used for other
purposes.
	}
	version { $Id$ }
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
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
	start {
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
