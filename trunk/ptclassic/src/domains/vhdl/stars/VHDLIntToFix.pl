defstar {
	name { IntToFix }
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
	desc { Convert an integer input to a fixed-point output. }
	input {
		name { input }
		type { int }
		desc { Input integer type }
	}
	output {
		name { output }
		type { fix }
		desc { Output fix type }
	}
        defstate {
                name { OutputPrecision }
                type { precision }
                default { 16.0 }
                desc {
Precision of the output in bits.
The integer number is cast to a double and then converted to this precision.
If the value of the double cannot be represented by the number of bits
specified in the precision parameter, then the OverflowHandler will be called.
		}
        }
        protected {
		Fix outFix;
        }
	setup {
	  // Set the output port's precision according to OutputPrecision.
	  outFix = Fix(((const char *) OutputPrecision));
	  Precision outPrecision(outFix);
	  output.setPrecision(outPrecision);
	}
	go {
	  StringList out;
	  int nbits, i;

	  nbits = outFix.fracb();
	  int fracFactor = 1;
	  for (i = 0 ; i < nbits ; i++) {
	    fracFactor *= 2;
	  }

	  nbits = (outFix.intb() + outFix.fracb() - 1);
	  int halfRange = 1;
	  for (i = 0 ; i < nbits ; i++) {
	    halfRange *= 2;
	  }

	  int fullRange = halfRange * 2;

	  // Input is a general integer.
	  // Output is a ranged integer.
	  // Scale up the integer by the fractional bit factor.
	  // Mod the result by the upper limit on the range.
	  out << "$ref(output) $assign(output) "
	      << "(((($ref(input) * " << fracFactor << ")"
	      << " + " << halfRange << ")"
	      << " mod " << fullRange << ")"
	      << " - " << halfRange << ")"
	      << ";\n";

	  addCode(out);
	  out.initialize();
	}
}
