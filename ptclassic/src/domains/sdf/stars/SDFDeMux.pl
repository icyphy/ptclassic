defstar {
	name { DeMux }
	domain { SDF }
	desc {
Demultiplexes one input onto any number of output streams.
The star consumes B particles from the input, where B is the blockSize.
These B particles are copied to exactly one output,
determined by the "control" input.  The other outputs get 0.0.
Integers from 0 through N-1 are accepted at the "control"
input, where N is the number of outputs.  If the control input is
outside this range, all outputs get 0.0.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	input {
		name {input}
		type {float}
	}
	input {
		name {control}
		type {int}
	}
	outmulti {
		name {output}
		type {float}
	}
	defstate {
		name {blockSize}
		type {int}
		default {1}
		desc {Number of particles in a block.}
	}
	start {
		input.setSDFParams(int(blockSize),int(blockSize)-1);
		MPHIter nexto(output);
		PortHole* p;
		while((p = nexto++) != 0)
		   ((SDFPortHole*)p)->setSDFParams(int(blockSize),int(blockSize)-1);
	}
	go {
	    int n = int(control%0);
	    MPHIter nexto(output);
	    PortHole* p;
	    for (int i = 0; i < output.numberPorts(); i++) {
		p = nexto++;
		int j;
		if (i == n)
		    for (j = int(blockSize)-1; j >= 0; j--)
		        (*p)%j = input%j;
		else
		    for (j = int(blockSize)-1; j >= 0; j--)
		        (*p)%j << 0.0;
	    }
	}
}
