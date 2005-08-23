defstar {
	name { Transpose }
	domain { SDF }
	desc {
Transpose a rasterized matrix (one that is read as a sequence
of particles, row by row, and written in the same form).
The number of particles produced and consumed equals the product
of "samplesInaRow" and "numberOfRows".
	}
	version {@(#)SDFTranspose.pl	1.9	3/7/96}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
	location { SDF main library }
	input {
		name { input }
		type { ANYTYPE }
	}
	output {
		name { output }
		type { =input }
	}
	defstate {
		name { samplesInaRow }
		type { int }
		default { 8 }
		desc { The number of input samples constituting a row. }
	}
	defstate {
		name { numberOfRows }
		type { int }
		default { 8 }
		desc { The number of rows in the input matrix.  }
	}
	protected {
		int size;
	}
	setup {
		size = int(samplesInaRow)*int(numberOfRows);
		input.setSDFParams(size, size-1);
		output.setSDFParams(size, size-1);
	}
	go {
	    for(int i=int(samplesInaRow)-1; i >= 0; i-- ) {
		for(int j=int(numberOfRows)-1; j >= 0; j-- ) {
		    output%(j+i*int(numberOfRows)) =
			input%(i+j*int(samplesInaRow));
		}
	    }
	}
}
