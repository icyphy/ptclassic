defstar {
	name { Repeat }
	domain { C50 }
	desc { Repeats each input sample the specified number of times. }
	version { $Id$ }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	explanation {
Repeat repeats each input Particle the specified number of times
(\fInumTimes\fR) on the output.  Note that this is a sample rate
change, and hence affects the number of invocations of downstream
stars.
.UH IMPLEMENTATION
We must be careful to avoid large interrupt latencies.  To be optimal
this requires the aid of the target.  For now, we just assume the
worst case.
	}
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {=input}
	}
	// FIXME: Does not support a blocksize parameter
	state {
		name {numTimes}
		type {int}
		default {2}
		desc { Repetition factor }
	}
	setup {
		output.setSDFParams(int(numTimes),int(numTimes)-1);
	}
	codeblock(cbOnce) {
    	splk	#$addr(input),BMAR		;Address input 		=>BMAR
    	bldd	BMAR,#$addr(output)		;output = input
	}
	codeblock(cbRepLoop) {
	mar     *,AR0				;
	lar	AR0,#$addr(input)		;Address input		=> AR0
    	lar	AR7,#$addr(output)		;Address output		=> AR7
    	lacc	*,0,AR7				;Accu = input
    	rpt	#$val(numTimes)-1		;for number of numTimes
    	  sacl	*				;outputsample(i) = input
	}
 
	go {
		// should fork star if there is we only repeat by one input
		if ( int(numTimes) == 1 ) addCode(cbOnce);
		else addCode(cbRepLoop);
	}

	exectime {
		return int(numTimes)+4;
	}
}
