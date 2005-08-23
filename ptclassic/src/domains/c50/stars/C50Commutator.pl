defstar {
	name { Commutator }
	domain { C50 }
	desc { 
Interleaves the two input signals.
 }
	version { $Id$ }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 control library }
	explanation {
.Ir "interleaving data streams"
.Ir "combining data streams"
Takes N input streams and synchronously combines them into one output stream,
where N is the number of inputs.  It consumes B input particles from each
input, and produces N*B particles on the output, where B = blockSize.
The first B particles on the output come from the first input,
the next B particles from the next input, etc.

	}
	inmulti {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {=input}
	}
        state {
                name {blockSize}
                type {int}
                default {1}
                desc {Number of particles in a block.}
	}
        state  {
                name { inputNum }
                type { int }
                default { 0 }
                desc { input#() }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }
        setup {
                int n = input.numberPorts();
		int bs = int(blockSize);
		input.setSDFParams(bs,bs-1);
		output.setSDFParams(n*bs,n*bs-1);
        }
        codeblock (one) {
        mar	*,AR7				;
	lar	AR7,#$addr(input#1)        	;just move data from in to out
        bldd    *,#$addr(output)		;
        }

 	codeblock(main) {
        lar	AR0,#$addr(output)		;Address output		=> AR0
	mar 	*,AR0				;
        }
        codeblock(loop) {
        bldd    #$addr(input#inputNum),*	;input values to output stream
        }

	go {
                if (input.numberPorts() == 1) {
                        addCode(one);
                        return;
                }
		addCode(main);
                for (int i = 1; i <= input.numberPorts(); i++) {
                        inputNum=i;
                        addCode(loop);
                }
	}
	exectime {
		return (2*(int(input.numberPorts()))+2) ;
	}
}






