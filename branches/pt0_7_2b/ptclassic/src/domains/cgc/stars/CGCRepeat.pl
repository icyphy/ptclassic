defstar {
	name {Repeat}
	domain {CGC}
	desc {  Repeats each input sample the specified number of times. }
	version {$Id$}
	author { S. Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
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
	constructor {
		noInternalState();
	}
	setup {
		output.setSDFParams(int(numTimes),int(numTimes)-1);
	}
	codeblock (out) {
	int i;
	for (i = 0; i < $val(numTimes); i++) {
		$ref(output, i) = $ref(input);
	}
	}
	go {
		addCode(out);
	}
	exectime {
		return int(numTimes);
	}
}
