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
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
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
	constructor {
		noInternalState();
	}
	setup {
		if ( int(blockSize) < 1 ) {
		 	Error::abortRun(*this, "blockSize must be positive");
			return;
		}
		output.setSDFParams(int(blockSize),int(blockSize)-1);
		input.setSDFParams(int(blockSize),int(blockSize)-1);
	}
	codeblock(init) {
	int n = $ref(control);
	int j = $val(blockSize);
	}
	codeblock(switchStatement) {
	switch(n)
	}
	codeblock(copydata,"int i, int portnum") {
	    case @i:
		while (j--) {
			$ref(output,j) = $ref(input#@portnum,j);
		}
		break;
	}
	codeblock(badPortNum) {
	    default:
		fprintf(stderr, "invalid control input %d", n);
	}
	initCode {
		addInclude("<stdio.h>");
	}
	go {
		addCode(init);
		addCode(switchStatement);
		addCode("\t{\n");
		// control value i means port number i+1
		for (int i = 0; i < input.numberPorts(); i++) {
			addCode(copydata(i,i+1));
		}
		addCode(badPortNum);
		addCode("\t}\n");
	}
	exectime {
		return int(blockSize) + 3;  
	}
}
