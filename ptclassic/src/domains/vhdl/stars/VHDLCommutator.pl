defstar {
	name { Commutator }
	domain { VHDL }
	version { $Id$ }
	desc {
Takes N input streams (where N is the number of inputs) and
synchronously combines them into one output stream.
It consumes B input particles from each
input (where B is the blockSize), and produces N*B particles on the
output.
The first B particles on the output come from the first input,
the next B particles from the next input, etc.
	}
	author { Michael C. Williamson, J. T. Buck and E. A. Lee}
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	inmulti {
		name { input }
		type { ANYTYPE }
	}
	output {
		name { output }
		type { =input }
	}	
	defstate {
		name { blockSize }
		type { int }
		default { 1 }
		desc { Number of particles in a block. }
	}
	setup {
		int n = input.numberPorts();
		input.setSDFParams(int(blockSize),int(blockSize)-1);
		output.setSDFParams(n*int(blockSize),n*int(blockSize)-1);
	}
	go {
	    StringList out;

	    //	    MPHIter nexti(input);
	    //	    PortHole* p;
	    for (int i = input.numberPorts()-1; i >= 0; i--) {
		//		p = nexti++;
		for (int j = int(blockSize)-1; j >= 0; j--) {
		    out << "$ref(output, ";
		    out << -(j+i*int(blockSize));
		    out << ") $assign(output) $ref(input#";
		    out << i+1;
		    out << ", ";
		    out << -j;
		    out << ");\n";

		    // output%(j+i*int(blockSize)) = (*p)%j;
		}
	    }

	    addCode(out);
	    out.initialize();
	}
}

