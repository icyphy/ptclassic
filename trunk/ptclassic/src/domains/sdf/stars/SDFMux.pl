defstar {
	name { Mux }
	domain { SDF }
	desc {
Multiplexes any number of inputs onto one output stream.
B particles are consumed on each input, where B is the blockSize.
But only one of these blocks of particles is copied to the output.
The one copied is determined by the "control" input.
Integers from 0 through N-1 are accepted at the "control" input,
where N is the number of inputs.  If the control input is outside
this range, an error is signaled.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	inmulti {
		name {input}
		type {ANYTYPE}
	}
	input {
		name {control}
		type {int}
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
		output.setSDFParams(int(blockSize),int(blockSize)-1);
		input.setSDFParams(int(blockSize),int(blockSize)-1);
	}
	go {
	    int n = int(control%0);
	    MPHIter nexti(input);
	    PortHole* p;
	    for (int i = 0; i < input.numberPorts(); i++) {
		p = nexti++;
		if (i == n) {
		    for (int j = int(blockSize)-1; j >= 0; j--)
		        output%j = (*p)%j;
		    return;
		}
	    }
	    Error::abortRun(*this,
		"Invalid control input to Mux - out of range");
	}
}
