defstar {
	name { Commutator }
	domain { CG56 }
	desc { 
Interleaves the two input signals.
 }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
.Ir "interleaving data streams"
.Ir "combining data streams"
Takes N input streams and synchronously combines them into one output stream,
where N is the number of inputs.  It consumes N input particles from each
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
        start {
                int n = input.numberPorts();
                MPHIter nexti(input);
                PortHole* p;
                while((p = nexti++) != 0)
 ((SDFPortHole*)p)->setSDFParams(int(blockSize),int(blockSize)-1);
                output.setSDFParams(n*int(blockSize),n*int(blockSize)-1);
        }
        codeblock (one) {
        move    $ref(input#1),x0        ; just move data from in to out
        move    x0,$ref(output)
        }

 	codeblock(main) {
        move    #$addr(output),r1
	nop
        }
        codeblock(loop) {
        move    $ref(input#inputNum),x0
        clr     a               x0,x:(r1)+
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
		return (2*(int(input.numberPorts()))+1) ;
	}
}
