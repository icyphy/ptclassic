defstar {
	name { Distributor }
	domain { CGC }
	version {$Id$ }
	desc {
Takes one input stream and synchronously splits it into N output streams,
where N is the number of outputs.  It consumes N*B input particles,
where B = blockSize, and sends the first B particles to the first output,
the next B particles to the next output, etc.
	}
	author { E. A. Lee}
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
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
	state {
		name {ix}
		type { int }
		default { 1 }
		desc { index for multiple output trace }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	start {
		int n = output.numberPorts();
		input.setSDFParams(n*int(blockSize),n*int(blockSize)-1);
		output.setSDFParams(int(blockSize),int(blockSize)-1);
	}
// note: delay 0 is the newest sample, so we must read backwards
	go {
		StringList out;
		if(int(blockSize) > 1) out << "\tint j;\n";
		for (int i = output.numberPorts() - 1; i >= 0; i--) {
		   ix = output.numberPorts() - i;
		   if(int(blockSize) > 1) {
			out << "\tfor (j = ";
			out << int(blockSize)-1;
			out << "; j >= 0; j--)\n";
			out << "\t\t$ref2(output#ix,j) = $ref2(input,j+";
			out << i*int(blockSize);
		   } else {
			out << "\t$ref2(output#ix,0) = $ref2(input,";
			out << i;
		   }
		   out << ");\n";
		   addCode(out);
		   out.initialize();
		}
	}
}

