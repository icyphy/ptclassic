defstar {
	name { Repeat }
	domain { CG56 }
	desc { Repeats each input sample the specified number of times. }
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
Repeat repeats each input Particle the specified number of times
(\fInumTimes\fR) on the output.  Note that this is a sample rate
change, and hence affects the number of invocations of downstream
stars.
	}
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {=input}
	}
	state {
		name {numTimes}
		type {int}
		default {2}
		desc { Repetition factor. }
	}
	start {
		output.setSDFParams(int(numTimes),int(numTimes)-1);
	
	}
	codeblock(block) {
	move	#$addr(output),r1
	nop
	move	$ref(input),x0
	}
	codeblock(repeat) {
	rep	#$val(numTimes)
	}
	codeblock(cont) {
	move	x0,x:(r1)+
        }
 	go {
   		gencode(block);
		if (int(numTimes)>1) gencode(repeat);
		gencode(cont);
 	}

	exectime {
		return int(numTimes)+4;
 	}
}
 