defstar {
	name { Dummy }
	domain { CG56 }
	desc { 
Interleaves the two input signals.
 }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
Takes N input streams and synchronously combines them into one output stream,
where N is the number of inputs.  It consumes N input particles from each
input, and produces N*B particles on the output, where B = blockSize.
The first B particles on the output come from the first input,
the next B particles from the next input, etc.
	}
	inmulti {
		name {input}
		type {FIX}
	}
	output {
		name {output}
	        type {FIX}
	}
        state {
                name {blockSize}
                type {int}
                default {1}
                desc {Number of particles in a block.}
	}
  	codeblock(main) {
        move    #$addr(output),r1
        move    $ref(input#1),x0
        clr     a               x0,x:(r1)+
        move    $ref(input#2),x0
        clr     a               x0,x:(r1)+
	}
	go {
		gencode(main);
	}
	exectime {
		return 5;
	}
}
