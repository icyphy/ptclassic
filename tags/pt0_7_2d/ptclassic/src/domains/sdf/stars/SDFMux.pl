defstar {
	name { Mux }
	domain { SDF }
	desc {
Multiplex any number of inputs onto one output stream.  At each
firing, blockSize particles are consumed on each input port, but
only one of these blocks of particles is copied to the output.
The one copied is determined by the "control" input.
Integers from 0 through N-1 are accepted at the "control" input,
where N is the number of inputs.  If the control input is outside
this range, an error is signaled.
	}
	version {@(#)SDFMux.pl	1.16	10/01/96}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
<a name="multiplex"></a>
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
		int j = int(blockSize);
		if (n >= 0 && n < numports) {
			MPHIter nexti(input);
			PortHole* p = 0;
			for (int i = 0; i <= n; i++) {
				p = nexti++;
			}
			while (j--) {
				output%j = (*p)%j;
			}
		}
		else {
			StringList msg = "Control input ";
			msg << n << " is out of the range [0,"
			    << (numports - 1) << "]";
			Error::abortRun(*this, msg);
			return;
		}
	}
}
