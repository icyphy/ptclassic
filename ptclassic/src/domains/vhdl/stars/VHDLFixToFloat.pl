defstar {
	name { FixToFloat }
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
	desc { Convert a fixed-point input to a float output. }
	input {
		name { input }
		type { fix }
	}
	output {
		name { output }
		type { float }
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
	  int factor = 1;
	  for (int i = 0 ; i < nbits ; i++) {
	    factor *= 2;
	  }

	  // Input has a fractional part.
	  // Output is a general real.
	  // Divide down by the fractional size to make it a real.
	  out << "$ref(output) $assign(output) "
	      << "REAL ($ref(input)) / REAL (" << factor << ")"
	      << ";\n";

	  addCode(out);
	  out.initialize();
	}
}
