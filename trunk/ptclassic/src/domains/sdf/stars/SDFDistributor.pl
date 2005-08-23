defstar {
	name { Distributor }
	domain { SDF }
	version {@(#)SDFDistributor.pl	2.10	3/2/95}
	desc {
Takes one input stream and synchronously splits it into N output streams,
where N is the number of outputs.  It consumes N*B input particles,
where B = blockSize, and sends the first B particles to the first output,
the next B particles to the next output, etc.
	}
	author { J. T. Buck and E. A. Lee}
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
		name { input }
		type { ANYTYPE }
	}
	outmulti {
		name { output }
		type { =input }
	}
	defstate {
		name {blockSize}
		type {int}
		default {1}
		desc {Number of particles in a block.}
	}
	setup {
		int n = output.numberPorts();
		input.setSDFParams(n*int(blockSize),n*int(blockSize)-1);
		output.setSDFParams(int(blockSize),int(blockSize)-1);
	}
// note: delay 0 is the newest sample, so we must read backwards
	go {
		MPHIter nexto(output);
		PortHole* p;
		for (int i = output.numberPorts() - 1; i >= 0; i--) {
		   p = nexto++;
		   for (int j = int(blockSize)-1; j >= 0; j--)
			(*p)%j = input%(j+i*int(blockSize));
		   }
	}
}

