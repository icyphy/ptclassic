defstar {
	name { Distributor }
	domain { CGC }
	version {@(#)CGCDistributor.pl	1.7	7/11/96 }
	desc {
Takes one input stream and synchronously splits it into N output streams,
where N is the number of outputs.  It consumes N*B input particles,
where B = blockSize, and sends the first B particles to the first output,
the next B particles to the next output, etc.
	}
	author { E. A. Lee}
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
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
		StringList out;
		if(int(blockSize) > 1) out << "\tint j;\n";
		for (int i = output.numberPorts() - 1; i >= 0; i--) {
		   int port = output.numberPorts() - i;
		   if(int(blockSize) > 1) {
			out << "\tfor (j = " << int(blockSize)-1
			    << "; j >= 0; j--)\n" << "\t\t$ref2(output#"
			    << port << ",j) = $ref2(input,j+"
			    << i*int(blockSize) << ");\n";
		   } else {
			out << "\t$ref2(output#" << port
			    << ",0) = $ref2(input," << i << ");\n";
		   }
		}
		addCode(out);
	}
	exectime {
		return int(blockSize)*2*output.numberPorts();
	}
}

