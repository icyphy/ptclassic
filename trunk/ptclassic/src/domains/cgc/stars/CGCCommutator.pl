defstar {
	name {Commutator}
	domain {CGC}
	version {$Id$ }
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
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
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
	state {
		name {ix}
		type { int }
		default { 1 }
		desc { index for multiple output trace }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
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
		    ix = input.numberPorts() - i;
		    if(int(blockSize) > 1) {
			out << "\tfor (j = ";
			out << int(blockSize)-1;
			out << "; j >= 0; j--)\n";
			out << "\t\t$ref2(output,j+";
			out << i*int(blockSize);
			out << ") = $ref2(input#ix,j";
		   } else {
			out << "\t$ref2(output,";
			out << i;
			out << ") = $ref2(input#ix,0";
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

