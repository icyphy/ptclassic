defstar {
	name { DeMux }
	domain { CGC }
	desc {
Demultiplexes one input onto any number of output streams.
The star consumes B particles from the input, where B is the blockSize.
These B particles are copied to exactly one output,
determined by the "control" input.  The other outputs get a zero of the
appropriate type.

Integers from 0 through N-1 are accepted at the "control"
input, where N is the number of outputs.  If the control input is
outside this range, all outputs get zero.
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
<a name="demultiplex"></a>
	}
	input {
		name {input}
		type {anytype}
	}
	input {
		name {control}
		type {int}
	}
	outmulti {
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
		input.setSDFParams(int(blockSize),int(blockSize)-1);
		output.setSDFParams(int(blockSize),int(blockSize)-1);
	}
        codeblock(init) {
	int n = $ref(control);
	int j = $val(blockSize);
	}
	codeblock(nonComplexCopyData, "int i, int portnum") {
		/* Output port #@portnum */
		if (n != @i) $ref(output#@portnum,j) = 0;
		else $ref(output#@portnum,j) = $ref(input,j);
	}
	codeblock(complexCopyData, "int i, int portnum") {
		/* Output port #@portnum */
		if (n != @i) {
		  $ref(output#@portnum,j).real = 0;
		  $ref(output#@portnum,j).imag = 0;
		}
		else $ref(output#@portnum,j) = $ref(input,j);
	}
	codeblock(blockIterator) {
	while (j--)
	}
	go {
		addCode(init);
		addCode(blockIterator);
		addCode("\t{\n");
		// control value i means port number i+1
		for (int i = 0; i < output.numberPorts(); i++) {
		  if (input.resolvedType() == COMPLEX) 
		    addCode(complexCopyData(i,i+1));
		  else
		    addCode(nonComplexCopyData(i,i+1));
		}
		addCode("\t}\n");
	}
	exectime {
		return 1 + (output.numberPorts() + 1) * int(blockSize);
	}
}
