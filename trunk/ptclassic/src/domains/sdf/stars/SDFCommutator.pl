defstar {
	name {Commutator}
	domain {SDF}
	version {$Id$}
	desc {
Takes N input streams (where N is the number of inputs) and
synchronously combines them into one output stream.
It consumes B input particles from each
input (where B is the blockSize), and produces N*B particles on the
output.
The first B particles on the output come from the first input,
the next B particles from the next input, etc.
	}
	author { J. T. Buck and E. A. Lee}
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	inmulti {
		name {input}
		type {ANYTYPE}
	}
	output {
		name {output}
		type {=input}
	}	
	defstate {
		name {blockSize}
		type {int}
		default {1}
		desc {Number of particles in a block.}
	}
	start {
		int n = input.numberPorts();
		MPHIter nexti(input);
		PortHole* p;
		while((p = nexti++) != 0)
		   ((SDFPortHole*)p)->setSDFParams(int(blockSize),int(blockSize)-1);
		output.setSDFParams(n*int(blockSize),n*int(blockSize)-1);
	}
	go {
		MPHIter nexti(input);
		PortHole* p;
		for (int i = input.numberPorts()-1; i >= 0; i--) {
		    p = nexti++;
		    for (int j = int(blockSize)-1; j >= 0; j--)
			output%(j+i*int(blockSize)) = (*p)%j;
		    }
	}
}

