defstar {
	name { Inhomogeneous }
	domain { VHDL }
	desc { Input and output settable numbers of tokens.  }
	version { $Id$ }
	author { Michael C. Williamson }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	defstate {
	        name { transfer }
		type { int }
		default { 1 }
	}
	setup {
	        input.setSDFParams(int(transfer), int(transfer)-1);
	        output.setSDFParams(int(transfer), int(transfer)-1);
        }
	go {
	  StringList out;
	  for (int i=0; i<int(transfer); i++) {
	    out << "$ref(output, ";
	    out << i;
	    out << ") $assign(output) $ref(input, ";
	    out << i;
	    out << ");\n";
	  }
	  addCode(out);
	  out.initialize();
	}
	exectime {
		return int(transfer);
	}
}
