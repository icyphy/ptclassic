defstar {
	name { Select }
	domain { VHDL }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
	author { Michael C. Williamson, J. T. Buck }
	location { VHDL main library }
	desc {
This star requires a BDF scheduler!

If the value on the 'control' line is nonzero, trueInput
is copied to the output; otherwise, falseInput is.
	}
	input {
		name { trueInput }
		type { ANYTYPE }
	}
	input {
		name { falseInput }
		type { =trueInput }
	}
	input {
		name { control }
		type { int }
	}
	output {
		name { output }
		type { =trueInput }
	}
	method {
		name { notone }
		type { int }
		arglist { "(VHDLPortHole& port)" }
		code {
			return (port.numInitDelays() > 1 ||
				port.far()->numXfer() > 1);
		}
	}
	setup {
		// all connected buffers must be size 1, for now.
		// need to check this.
		// FIXME: we can permit output to be > 1 by generating
		// a copy of the (shared) input buffer to the output.
		if (notone(output) || notone(trueInput) ||
		    notone(falseInput) || notone(control))
			Error::abortRun(*this,
		"Non-unity buffers connected to a Select not yet supported");
		else {
			// make all the buffers overlap.
			output.embed(trueInput,0);
			output.embed(falseInput,0);
			trueInput.setRelation(DF_TRUE,&control);
			falseInput.setRelation(DF_FALSE,&control);
		}
	}
	codeblock (std) {
$ref(output) $assign(output) $ref(trueInput);
$ref(output) $assign(output) $ref(falseInput);
	}
	go {
	  addCode(std);
	}
	exectime { return 0;}
}



