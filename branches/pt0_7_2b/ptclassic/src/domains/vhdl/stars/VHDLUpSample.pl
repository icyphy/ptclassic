defstar {
	name {UpSample}
	domain {VHDL}
	desc { 
Upsample by a given "factor" (default 2), filling with "fill" (default 0.0).
The "phase" parameter tells where to put the sample in an output block.
The default is to output it first (phase = 0). The maximum phase is "factor" - 1.
Although the "fill" parameter is a floating-point number, if the input is of
some other type, such as complex, the fill particle will be obtained by casting
"fill" to the appropriate type.
	}
	version { @(#)VHDLUpSample.pl	1.5 03/07/96 }
	author { Michael C. Williamson, J. T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	input {
		name{input}
		type{ANYTYPE}
	}
	output {
		name{output}
		type{=input}
	}
	defstate {
		name {factor}
		type {int}
		default {2}
		desc { Number of samples produced. }
	}
	defstate {
		name {phase}
		type {int}
		default {0}
		desc { Where to put the input in the output block. }
	}
	defstate {
		name {fill}
		type {float}
		default {0.0}
		desc { Value to fill the output block. }
	}
	setup {
		output.setSDFParams(int(factor),int(factor)-1);
		if (int(phase) >= int(factor))
			Error::abortRun(*this, ": phase must be < factor");
	}
	go {
	  StringList out;
	  for (int i = 0; i < int(factor); i++) {
	    // FIXME: Will not work for COMPLEX type
	    out << "$ref(output, ";
	    out << -i;
	    out << ") $assign(output) ";
	    if (i == int(phase)) {
	      out << "$ref(input, 0)";
	    }
	    else {
	      float fillFloat = double(fill);
	      int fillInt = (int) fillFloat;
	      out << (strcmp(input.resolvedType(),INT)==0?
		      fillInt:fillFloat);
	      // yes, the previous statement really does work correctly
	      // even if the type of the output is resolved to COMPLEX
	      // or INT.  The appropriate conversion is performed.
	    }
	    out << ";\n";
	  }
	  addCode(out);
	}
}


