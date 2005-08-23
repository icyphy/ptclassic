defstar {
	name { Distributor }
	domain { VHDL }
	version { @(#)VHDLDistributor.pl	1.3 03/11/96 }
	desc {
Takes one input stream and synchronously splits it into N output streams,
where N is the number of outputs.  It consumes N*B input particles,
where B = blockSize, and sends the first B particles to the first output,
the next B particles to the next output, etc.
	}
	author { Michael C. Williamson, J. T. Buck and E. A. Lee}
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	input {
		name { input }
		type { ANYTYPE }
	}
	outmulti {
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
		int n = output.numberPorts();
		input.setSDFParams(n*int(blockSize),n*int(blockSize)-1);
		output.setSDFParams(int(blockSize),int(blockSize)-1);
	}
// note: delay 0 is the newest sample, so we must read backwards
	go {
	  StringList out;

	  if (strcmp(input.resolvedType(), "COMPLEX") == 0) {
	    for (int i = output.numberPorts() -1 ; i >= 0 ; i--) {
	      for (int j = int(blockSize)-1 ; j >= 0 ; j--) {
		out << "$refCx(output#";
		out << i+1;
		out << ", ";
		out << -j;
		out << ", real) $assign(output) $refCx(input, ";
		out << -(j+i*int(blockSize));
		out << ", real);\n";

		out << "$refCx(output#";
		out << i+1;
		out << ", ";
		out << -j;
		out << ", imag) $assign(output) ";
		out << double(0.0);
		out << ";\n";
	      }
	    }
	  }
	  else {
	    for (int i = output.numberPorts() -1 ; i >= 0 ; i--) {
	      for (int j = int(blockSize)-1 ; j >= 0 ; j--) {
		out << "$ref(output#";
		out << i+1;
		out << ", ";
		out << -j;
		out << ") $assign(output) $ref(input, ";
		out << -(j+i*int(blockSize));
		out << ");\n";
	      }
	    }
	  }

	  addCode(out);
	  out.initialize();
	}
}
