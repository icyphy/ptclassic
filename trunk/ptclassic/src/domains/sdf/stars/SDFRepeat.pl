defstar {
	name {Repeat}
	domain {SDF}
	desc {  Repeat each input sample a specified number of times. }
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
	defstate {
		name {blockSize}
		type {int}
		default {1}
		desc {Number of particles in a block.}
	}
	setup {
		input.setSDFParams(int(blockSize), int(blockSize)-1);
		output.setSDFParams(int(numTimes)*int(blockSize),
			int(numTimes)*int(blockSize)-1);
	}
	go {
	    for (int i = 0; i < int(numTimes); i++) {
		for (int j = int(blockSize)-1; j >= 0; j--)
		    output%(j+i*int(blockSize)) = input%j;
	    }
	}
}
