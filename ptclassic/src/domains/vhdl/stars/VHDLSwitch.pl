defstar {
	name { Switch }
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
This star requires a BDF scheduler.
Switches input events to one of two outputs, depending on
the value of the control input.  If control is true, the
value is written to trueOutput; otherwise it is written to
falseOutput.
	}
	input {
		name { input }
		type { ANYTYPE }
	}
	input {
		name { control }
		type { int }
	}
	output {
		name { trueOutput }
		type { =input }
	}
	output {
		name { falseOutput }
		type { =input }
	}
	constructor {
		trueOutput.setRelation(DF_TRUE, &control);
		falseOutput.setRelation(DF_FALSE, &control);
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
		// FIXME: we can permit input to be > 1 by generating
		// a copy of the input buffer to the (shared) output.
		if (notone(input) || notone(trueOutput) ||
		    notone(falseOutput) || notone(control))
			Error::abortRun(*this,
		"Non-unity buffers connected to a switch not yet supported");
		else {
			// make all the buffers overlap.
			input.embed(trueOutput,0);
			input.embed(falseOutput,0);
			trueOutput.setRelation(DF_TRUE, &control);
			falseOutput.setRelation(DF_FALSE, &control);
		}
	}
	codeblock (std) {
$ref(trueOutput) $assign(trueOutput) $ref(input);
$ref(falseOutput) $assign(falseOutput) $ref(input);
	}
	go {
	  addCode(std);
	}
	exectime { return 0;}
}
