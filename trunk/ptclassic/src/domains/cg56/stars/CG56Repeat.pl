defstar {
    name { Repeat }
    domain { CG56 }
    desc { Repeats each input sample the specified number of times. }
    version { $Id$ }
    author { Kennard White, Chih-Tsung Huang }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 control library }
    explanation {
Repeat repeats each input the specified number of times
(\fInumTimes\fR) on the output.
Note that this is a sample rate change, and hence affects the number
of invocations of downstream stars.
.UH IMPLEMENTATION
We must be careful to avoid large interrupt latencies.
To be optimal, this requires the aid of the target.
For now, we just assume the worst case.
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
    setup {
	    output.setSDFParams(int(numTimes),int(numTimes)-1);
    
    }
    codeblock(cbOnce) {
    	move	$ref(input),a
    	move	a,$ref(output)
    }
    codeblock(cbRepLoop) {
    	move	#$addr(output),r1
    	move	$ref(input),a
    	rep	#$val(numTimes)
    	  move	a,$mem(output):(r1)+
    }
    codeblock(cbDoLoop) {
    	move	#$addr(output),r1
    	move	$ref(input),a
    	.LOOP	#$val(numTimes)
    	  move	a,$mem(output):(r1)+
	.ENDL
	nop
    }

    go {
	/*IF*/ if ( int(numTimes) == 1 ) {
	    addCode(cbOnce);	// should fork buf instead
	} else if ( int(numTimes) <= 100 ) {
	    addCode(cbRepLoop);
	} else {
	    addCode(cbDoLoop);
	}
    }

    exectime {
	    return int(numTimes)+4;
    }
}
