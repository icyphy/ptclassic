defstar {
	name { Transpose }
	domain { SDF }
	desc {
Transposes a rasterized matrix.
Number of samples produced and consumed equals the product
of "samplesInaRow" and "numberOfRows".
	}
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
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
	start {
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
