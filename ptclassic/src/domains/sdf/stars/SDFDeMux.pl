defstar {
	name { DeMux }
	domain { SDF }
	desc {
Demultiplexes one input onto any number of output streams.
The star consumes B particles from the input, where B is the blockSize.
These B particles are copied to exactly one output,
determined by the "control" input.  The other outputs get a zero of the
appropriate type.

Integers from 0 through N-1 are accepted at the "control"
input, where N is the number of outputs.  If the control input is
outside this range, all outputs get zero.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
.Id "demultiplex"
	}
	input {
		name {input}
		type {anytype}
	}
	input {
		name {control}
		type {int}
	}
	outmulti {
		name {output}
		type {anytype}
	}
	defstate {
		name {blockSize}
		type {int}
		default {1}
		desc {Number of particles in a block.}
	}
	setup {
		input.setSDFParams(int(blockSize),int(blockSize)-1);
		output.setSDFParams(int(blockSize),int(blockSize)-1);
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
			((*p)%j).initialize();
	    }
	}
}
