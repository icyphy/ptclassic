defstar {
	name { Distributor }
	domain { C50 }
	desc {
Takes one input stream and synchronously splits it into N output streams,
where N is the number of outputs.  It consumes N*B input particles,
where B = blockSize, and sends the first B particles to the first output,
the next B particles to the next output, etc.
	}
	version { $Id$ }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	explanation {
.Ir "alternating data streams"
Distributes an input signal among two outputs, alternating samples.
	}
	input {
		name {input}
		type {ANYTYPE}
	}
	outmulti {
		name {output}
		type {=input}
	}
	state {
		name {blockSize}
		type {int}
		default {1}
		desc {Number of particles in a block.}
	}
	constructor {
		noInternalState();
	}
	setup {
		int n = output.numberPorts();
		int bs = int(blockSize);
		input.setSDFParams(n*bs,n*bs-1);
		output.setSDFParams(bs,bs-1);
	}
	codeblock (one) {
	splk    #$addr(input),BMAR		;just move data from in to out
	bldd    BMAR,#$addr(output#1)		;
	}

 	codeblock(main) {
	lar    	AR0,#$addr(input)		;Address input		=> AR0
	mar	*,AR0				;
	}
	codeblock(loop, "int i") {
	bldd    *,#$addr(output#@i)	;output port(n) = input(i)
	}

	go {
		if (output.numberPorts() == 1) {
			addCode(one);
		}
		else {
			addCode(main);
			for (int i = 1; i <= output.numberPorts(); i++) {
				addCode(loop(i));
			}
		}
	}
	exectime {
		return (2*(int(output.numberPorts()))+2);
	}
}
