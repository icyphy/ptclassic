defstar {
	name { Mux }
	domain { SDF }
	desc {
Multiplex any number of inputs onto one output stream.
B particles are consumed on each input, where B is the blockSize.
But only one of these blocks of particles is copied to the output.
The one copied is determined by the "control" input.
Integers from 0 through N-1 are accepted at the "control" input,
where N is the number of inputs.  If the control input is outside
this range, an error is signaled.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
.Id "multiplex"
	}
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
	setup {
		if ( int(blockSize) <= 0 ) {
			Error::abortRun(*this, "blockSize must be positive");
			return;
		}
		output.setSDFParams(int(blockSize),int(blockSize)-1);
		input.setSDFParams(int(blockSize),int(blockSize)-1);
	}
	go {
		int numports = input.numberPorts();
		int n = int(control%0);
		if (n < 0 || n >= numports) {
			StringList msg = "Control input ";
			msg << n << " is out of the range [0,"
			    << (numports - 1) << "]";
			Error::abortRun(*this, msg);
		}

		MPHIter nexti(input);
		PortHole* p = 0;
		for (int i = 0; i <= n; i++) {
			p = nexti++;
		}
		for (int j = int(blockSize)-1; j >= 0; j--) {
			output%j = (*p)%j;
		}
	}
}
