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
    location { CG56 main library }
    htmldoc {
Repeat repeats each input the specified number of times
(<i>numTimes</i>) on the output.
Note that this is a sample rate change, and hence affects the number
of invocations of downstream stars.
<h3>IMPLEMENTATION</h3>
We must be careful to avoid large interrupt latencies.
To be optimal, this requires the aid of the target.
For now, we just assume the worst case.
    }
    input {
	    name {input}
	    type {anytype}
	    attributes{P_CIRC}
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
    state {
	    name {blockSize}
	    type {int}
	    default {1}
	    desc { Size of a block of input data. }
    }

    protected{
	    int effectiveNumTimes;
	   // = numTimes*blockSize
    }

    setup {
	    effectiveNumTimes = int(numTimes)*int(blockSize);
	    input.setSDFParams(int(blockSize),int(blockSize)-1);
	    output.setSDFParams(effectiveNumTimes,effectiveNumTimes-1);    
    }
    codeblock(cbOnce) {
    	move	$ref(input),a
    	move	a,$ref(output)
    }
    codeblock(cbLoadAddr,"int block"){
    IF @(int(block))
    move #$addr(input),r2
    move #@(int(blockSize)-1),m2
    move #$addr(output),r1
    ELSE
    move #$addr(output),r1
    move $ref(input),a
    ENDIF
    }	   
    codeblock(cbRepLoop) {
    rep	 #$val(numTimes)
    move a,$mem(output):(r1)+
    }
    codeblock(cbDoLoop,"int block") {
    .LOOP	#@(effectiveNumTimes)
      IF @(int(block))
      move    $mem(input):(r2)+,a
      ENDIF
      move    a,$mem(output):(r1)+
    .ENDL
    nop
    }
    codeblock(cbRestoreModulo){
     move #$$0FFFF,m2
    }    
    go {
     int block;
     block = (int(blockSize) > 1);

	/*IF*/ if ( effectiveNumTimes == 1 ) {
	    addCode(cbOnce);	// should fork buf instead
	} else {
	   addCode(cbLoadAddr(block));
	   if ( effectiveNumTimes <= 100 && !(block)) {
			 addCode(cbRepLoop);
	   } else {
			 addCode(cbDoLoop(block));
	   }
	}
	if (block) addCode(cbRestoreModulo);
   }
    exectime {
	if (int(blockSize) > 1) return 2*effectiveNumTimes + 7;
	else if (int(numTimes)==1) return 2;
	else if (int(numTimes) > 100) return 3 + int(numTimes);
	else return 3 + 3 + int(numTimes);
    }
}
