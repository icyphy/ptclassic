defstar {
	name { Mux }
	domain { CGC }
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
	author { S. Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
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
	constructor {
		noInternalState();
	}
	setup {
		output.setSDFParams(int(blockSize),int(blockSize)-1);
		input.setSDFParams(int(blockSize),int(blockSize)-1);
	}
	go {
	    StringList out;
	    out << "\tint n, j;\n\tn = $ref(control);\n";

	    for (int i = 0; i < input.numberPorts(); i++) {
		out << "\t";
		if (i > 0) out << "else ";
		out << "if (n == " << i << ") {\n";
		out << "\t\tfor (j = $val(blockSize)-1; j >= 0; j--) \n";
		out << "\t\t\t$ref(output,j) = $ref(input#" << i+1 << ",j);\n";
		out << "\t}\n";
	    }
	    out << "\telse \n\t\tprintf(\"invalid control input to Mux\");\n";
	    addCode(out);
	}
	exectime {
		/* worst case */
		return int(blockSize) + input.numberPorts();  
	}
}
