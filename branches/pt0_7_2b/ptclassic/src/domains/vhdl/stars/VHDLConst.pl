defstar {
	name {Const}
	domain {VHDL}
	desc {
Output a constant signal with value given by the "level" parameter (default 0.0).
	}
	version {@(#)VHDLConst.pl	1.2 03/07/96}
	author { Michael C. Williamson, J. T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	output {
		name {output}
		type {float}
	}
	defstate {
		name {level}
		type{float}
		default {"0.0"}
		desc {The constant value.}
	}
	codeblock (std) {
$ref(output) $assign(output) $val(level);
	}
	go {
	  addCode(std);
	}
}

