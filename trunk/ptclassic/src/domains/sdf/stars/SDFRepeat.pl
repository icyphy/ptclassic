ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
                        All Rights Reserved.

Programmer:  E. A. Lee
Date of creation: 10/28/90

Repeat repeats each input Particle the specified number of times
(\fInumTimes\fR) on the output.

************************************************************************/
}
defstar {
	name {Repeat}
	domain {SDF}
	desc { "repeats each input sample the specified number of times" }
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
		desc { "Repetition factor" }
	}
	start {
		output.setSDFParams(int(numTimes),int(numTimes)-1);
	}
	go {
		for (int i = 0; i < int(numTimes); i++)
			output%i = input%0;
	}
}
