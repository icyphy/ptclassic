defstar {
	name { Distributor }
	domain { C50 }
	desc {
Takes one input stream and synchronously splits it into N output streams,
where N is the number of outputs.  It consumes N*B input particles,
where B = blockSize, and sends the first B particles to the first output,
the next B particles to the next output, etc.
	}
	version { @(#)C50Distributor.pl	1.8	05/26/98 }
	author { Luis Gutierrez, A. Baensch, ported from Gabriel, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<a name="alternating data streams"></a>
Distributes an input signal among two outputs, alternating samples.
	}

	outmulti {
		name {output}
		type {anytype}
	}

	input  {
		name {input}
		type {=output}
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
		// complex inputs)
		int effBlockSize;
		// number of input ports
		int numPorts;

		}

        setup {
		
                numPorts = output.numberPorts();
		int bs = int(blockSize);
		if (input.resolvedType() == COMPLEX) effBlockSize = 2*bs;
		else	effBlockSize = bs;
		output.setSDFParams(bs,bs-1);
		input.setSDFParams(numPorts*bs,numPorts*bs-1);
        }

        codeblock (one) {
	lmmr	ar1,#$addr(input)
	nop
	nop
	smmr	ar1,#$addr(output#1)
        }

	codeblock(oneCx){
	lmmr	ar1,#$addr(input,0)
	lmmr	ar2,#$addr(input,1)
	smmr	ar1,#$addr(output#1)
	smmr	ar2,#$addr(output#1)+1)
	}
	
	codeblock(moveBlockInit){
	mar	*,ar1
	}

	codeblock(moveBlock,"int Inum"){
	lar	ar1,#$addr(output#@Inum)
	rpt	#@(effBlockSize - 1)
	bldd	#($addr(input)+@(effBlockSize*(Inum-1))),*+
	}

	go {

		if ((numPorts == 1) && (effBlockSize == 2)) addCode(oneCx);
		else if ((numPorts == 1) && (effBlockSize == 1)) addCode(one);
		else {
			addCode(moveBlockInit);
                	for (int i = 1; i <= numPorts; i++) {
				addCode(moveBlock(i));
                	}
		}
	}

	exectime {
		if ((numPorts == 1) && (effBlockSize == 2))  return 4;
		else if ((numPorts == 1) && (effBlockSize == 2)) return 2;
		else {
			int onePortCost = effBlockSize + 2;
			return (onePortCost*numPorts + 1) ;
		}
	}
}
