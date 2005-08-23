defstar {
	name { SubInt }
	domain { VHDL }
	desc { Output the "pos" input minus all "neg" inputs }
	version { @(#)VHDLSubInt.pl	1.3 03/07/96 }
	author { Michael C. Williamson, A. P. Kalavade }
	copyright { 
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	input {
		name{ pos }
		type{ int }
	}
	inmulti {
		name{ neg }
		type{ int }
	}
	output {
		name{ output }
		type{ int }
	}
        constructor {
                noInternalState();
        }
	codeblock (std) {
$ref(output) $assign(output) $ref(pos) - ($interOp(+, neg));
	}
        go {
	  addCode(std);
        }
}
