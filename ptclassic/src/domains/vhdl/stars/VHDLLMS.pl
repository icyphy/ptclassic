defstar {
	name { LMS }
	domain { VHDL }
	desc { 
Adaptive filter using LMS adaptation algorithm. 
}
	version { $Id$ }
	author { M.C. Williamson, E.A. Lee, A.P. Kalavade }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	input {
		name { signalIn }
		type { float }
	}
	input {
                name { error }
                type { float }
        }
	output {
		name { signalOut }
		type { float }
	}
	defstate {
		name {taps}
		type {floatarray}
		default {
	"-.040609 -.001628 .17853 .37665 .37665 .17853 -.001628 -.040609"
		}
		desc { Filter tap values. }
	}
        defstate {
                name { stepSize }
                type { float }
                default { "0.01" }
                desc { Adaptation step size. }
        }
	go {
	  StringList out;
	  int tapSize = taps.size();

	  // Perform calculation.
	  int j = 0;
	  for (int i = 0; i < int(tapSize); i++) {
	      if (i%10 == 0) {
		j = i;
		if (i!=0) out << ";\n";
		StringList condText = "";
		condText << "$temp(signalOut" << i-10 << ",float) + ";
		out <<  "$temp(signalOut" << i << ",float) := "
		    << (i != 0 ? (const char*) condText : "") 
		    << "$ref(signalIn," << i << ")*$ref(taps," << i << ")";
	      }
	      else {
		out << "+ $ref(signalIn," << i << ")*$ref(taps," 
		    << i << ")";
	      }
	  }
	  out << ";\n";
	  for (int k = 0; k < int(tapSize); k++) {
	    // Update the tap values.
	    out << "$ref(taps," << k << ") $assign(taps) $ref(taps," << k
		<< ") + $val(stepSize) * $ref(error) * $ref(signalIn,"
		<< k << ");\n";
	  }
	  out << "$ref(signalOut) $assign(signalOut) $temp(signalOut"
	      << j << ",float);\n";

	  addCode(out);
	  out.initialize();
	}
}
