defstar {
	name { AddFix }	
	domain { VHDL }
	desc {
Output the sum of the fixed-point inputs as a fixed-point value.
	}
	version { $Id$ }
        author { Michael C. Williamson }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
        location { VHDL main library }
	inmulti {
		name { input }
		type { fix }
	}
	output {
		name { output }
		type { fix }
	}
        defstate {
                name { ArrivingPrecision }
                type {int}
                default {"YES"}
                desc { 
Flag that indicates whether or not to keep the precision of the arriving
particles as is:  YES keeps the same precision, and NO casts them to the
precision specified by the parameter "InputPrecision". } 
        }
        defstate {
                name { InputPrecision }
                type { precision }
                default { 2.14 }
                desc { 
Precision of the input in bits.
The input particles are only cast to this precision if the
parameter "ArrivingPrecision" is set to NO.
		}
        }
        defstate {
                name { OutputPrecision }
                type { precision }
                default { 2.14 }
                desc {
Precision of the output in bits and precision of the accumulation.
When the value of the accumulation extends outside of the precision,
the OverflowHandler will be called.
                }
        }
	hinclude { <stdio.h> }
        protected {
		Fix fixInput, fixOutput;
        }
	setup {
	  // Set the output port's precision according to OutputPrecision.
	  fixInput = Fix(((const char*) InputPrecision));
	  fixOutput = Fix(((const char*) OutputPrecision));

	  Precision outPrecision(fixOutput);
	  output.setPrecision(outPrecision);
	}
        go {
	  StringList out;

	  int inputFracBits = fixInput.fracb();
	  int outputFracBits = fixOutput.fracb();
	  int outputBits = fixOutput.intb() + fixOutput.fracb();

	  printf("inputFracBits:  %d\n", inputFracBits);
	  printf("outputFracBits:  %d\n", outputFracBits);

	  int k = 0;

	  int nbits = (outputBits - 1);
	  int halfRange = 1;
	  for (k = 0 ; k < nbits ; k++) {
	    halfRange *= 2;
	  }
	  int fullRange = halfRange * 2;

	  int inputDiv = 1;
	  for (k = 0 ; k < inputFracBits ; k++) {
	    inputDiv *= 2;
	  }

	  int outputDiv = 1;
	  for (k = 0 ; k < outputFracBits ; k++) {
	    outputDiv *= 2;
	  }

	  int maxDiv = max(inputDiv,outputDiv);

	  printf("inputDiv:   %d\n", inputDiv);
	  printf("outputDiv:  %d\n", outputDiv);
	  printf("maxDiv:     %d\n", maxDiv);

	  // Initialize accum.
	  out << "$temp(accum,int) := 0"
	      << ";\n";

	  // Rework this?
	  // Scale up each input into a common base of integers.
	  // Do the math in that base.
	  // Then take the result and divide it down into the
	  // output ranged integer's base.

	  // Get the input port with name input#i
	  for (int i = 0 ; i < input.numberPorts() ; i++) {
	    StringList portName = "input#";
	    portName << i+1;
	    out << "$temp(accum,int) := $temp(accum,int) + "
		<< "($ref(" << portName << ") * "
		<< maxDiv << "/" << inputDiv << ")"
		<< ";\n";
	  }
	  out << "$ref(output) $assign(output) "
	      << "(((($temp(accum,int) * "
	      << outputDiv << "/" << maxDiv <<")"
	      << " + " << halfRange << ")"
	      << " mod " << fullRange << ")"
	      << " - " << halfRange << ")"
	      << ";\n";

	  addCode(out);
	  out.initialize();
        }
}
