defstar {
	name {Commutator}
	domain {CGC}
	version {@(#)CGCCommutator.pl	1.6	7/11/96 }
	desc {
Takes N input streams (where N is the number of inputs) and
synchronously combines them into one output stream.
It consumes B input particles from each
input (where B is the blockSize), and produces N*B particles on the
output.
The first B particles on the output come from the first input,
the next B particles from the next input, etc.
	}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	inmulti {
		name {input}
		type {ANYTYPE}
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
		int n = input.numberPorts();
		input.setSDFParams(int(blockSize),int(blockSize)-1);
		output.setSDFParams(n*int(blockSize),n*int(blockSize)-1);
	}
	go {
		StringList out;
		if(int(blockSize) > 1) out << "\tint j;\n";
		for (int i = input.numberPorts()-1; i >= 0; i--) {
		    int port = input.numberPorts() - i;
		    if(int(blockSize) > 1) {
			out << "\tfor (j = ";
			out << int(blockSize)-1;
			out << "; j >= 0; j--)\n";
			out << "\t\t$ref2(output,j+";
			out << i*int(blockSize);
			out << ") = $ref2(input#" << port << ",j";
		   } else {
			out << "\t$ref2(output,";
			out << i;
			out << ") = $ref2(input#" << port << ",0";
		   }
		   out << ");\n";
		   addCode(out);
		   out.initialize();
		}
	}
	exectime {
		return int(blockSize)*2*input.numberPorts();
	}
}

