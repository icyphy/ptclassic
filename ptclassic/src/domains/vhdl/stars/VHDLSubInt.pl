defstar {
	name { SubInt }
	domain { VHDL }
	desc { Output the "pos" input minus all "neg" inputs }
	version { $Id$ }
	author { Michael C. Williamson, A. P. Kalavade }
	copyright { 
Copyright (c) 1990-%Q% The Regents of the University of California.
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
