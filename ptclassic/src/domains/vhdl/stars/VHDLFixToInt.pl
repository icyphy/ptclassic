defstar {
	name { FixToInt }
	domain { VHDL }
	version { $Id$ }
	author { Michael C. Williamson }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	desc { Convert a fixed-point input to an integer output. }
	input {
		name { input }
		type { fix }
		desc { Input fix type }
	}
	output {
		name { output }
		type { int }
		desc { Output int type }
	}
	hinclude { <stdio.h> }
        protected {
		Fix inFix;
        }
	setup {
	  // Use the sending port's precision as our input precision.
	  VHDLPortHole* inputPort =
	    (VHDLPortHole*) this->portWithName("input");
	  VHDLPortHole* sendingPort =
	    (VHDLPortHole*) inputPort->far();
	  Precision SendingPrecision = sendingPort->getPrecision();
	  //	  Precision InputPrecision = inputPort->getPrecision();
	  Precision InputPrecision = SendingPrecision;

	  printf("SendingPort:  int bits:  %d  frac bits:  %d\n",
		 SendingPrecision.intb(), SendingPrecision.fracb());
	  printf("InputPort:    int bits:  %d  frac bits:  %d\n",
		 InputPrecision.intb(), InputPrecision.fracb());

	  // Set the input port's precision according to InputPrecision.
	  inFix = Fix(InputPrecision);
	  //	  Precision inPrecision(inFix);
	  input.setPrecision(InputPrecision);
	}
	go {
	  StringList out;

	  int nbits = inFix.fracb();
	  int fracFactor = 1;
	  for (int i = 0 ; i < nbits ; i++) {
	    fracFactor *= 2;
	  }

	  // Input has a fractional part.
	  // Output is a general integer.
	  // Integer-divide down by the fractional size
	  // to make it an integer.
	  out << "$ref(output) $assign(output) "
	      << "$ref(input) / " << fracFactor
	      << ";\n";

	  addCode(out);
	  out.initialize();
	}
}
