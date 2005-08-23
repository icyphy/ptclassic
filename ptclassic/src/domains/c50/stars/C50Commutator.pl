defstar {
	name { Commutator }
	domain { C50 }
	desc { 
Interleaves the two input signals.
 }
	version {@(#)C50Commutator.pl	1.8	05/26/98}
	author { Luis Gutierrez, A. Baensch, ported from Gabriel, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<a name="interleaving data streams"></a>
<a name="combining data streams"></a>
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

	constructor {
		noInternalState();
	}
	
	protected {
		// holds the effective blocksize(to allow for
		// complex inputs
		int effBlockSize;
		// number of input ports
		int numPorts;

		}

        setup {
		
                numPorts = input.numberPorts();
		int bs = int(blockSize);
		if (output.resolvedType() == COMPLEX) effBlockSize = 2*bs;
		else	effBlockSize = bs;
		input.setSDFParams(bs,bs-1);
		output.setSDFParams(numPorts*bs,numPorts*bs-1);
        }

        codeblock (one) {
	lmmr	ar1,#$addr(input#1)
	nop	; required by pipeline
	nop
	smmr	ar1,#$addr(output)
        }

	codeblock(oneCx){
	lmmr	ar1,#$addr(input#1)
	lmmr	ar2,#($addr(input#1)+1)
	smmr	ar1,#$addr(output)
	smmr	ar2,#($addr(output)+1)
	}
	
	codeblock(moveBlockInit){
	mar	*,ar1
	}

	codeblock(moveBlock,"int Inum"){
	lar	ar1,#$addr(input#@Inum)
	rpt	#@(effBlockSize - 1)
	bldd	*+,#($addr(output)+@(effBlockSize*(Inum-1)))
	}

	go {

		if ((numPorts == 1) && (effBlockSize == 2)) {
			addCode(oneCx);
		} else if ((numPorts == 1) && (effBlockSize == 1)) {
			addCode(one);
		} else {
			addCode(moveBlockInit);
                	for (int i = 1; i <= numPorts; i++) {
				addCode(moveBlock(i));
                	}
		}
	}

	exectime {
		if ((numPorts == 1) && (effBlockSize == 2)) {
			 return 4;
		} else if ((numPorts == 1) && (effBlockSize == 2)) {
			return 2;
		} else {
			int onePortCost = effBlockSize + 2;
			return (onePortCost*numPorts + 1) ;
		}
	}
}






