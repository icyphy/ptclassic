defstar {
	name {Repeat}
	domain {SDF}
	desc {  Repeats each input sample the specified number of times. }
	version {$Revision$ $Date$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
Repeat repeats each input Particle the specified number of times
(\fInumTimes\fR) on the output.  Note that this is a sample rate
change, and hence affects the number of invocations of downstream
stars.
	}
	input {
		name{input}
		type{ANYTYPE}
	}
	output {
		name{output}
		type{=input}
	}
	defstate {
		name {numTimes}
		type {int}
		default {2}
		desc { Repetition factor. }
	}
	start {
		output.setSDFParams(int(numTimes),int(numTimes)-1);
	}
	go {
		for (int i = 0; i < int(numTimes); i++)
			output%i = input%0;
	}
}
