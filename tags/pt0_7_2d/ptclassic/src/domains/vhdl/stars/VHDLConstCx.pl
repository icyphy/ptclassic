defstar {
	name { ConstCx }
	domain { VHDL }
	desc {
Output a constant signal with value given by the "level" parameter (default 0.0).
	}
	version { @(#)VHDLConstCx.pl	1.2 03/07/96 }
	author { Michael C. Williamson, J. T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	output {
		name { output }
		type { complex }
	}
	defstate {
		name { real }
		type { float }
		default { "0.0" }
		desc {Real part of the constant value.}
	}
	defstate {
		name { imag }
		type { float }
		default { "0.0" }
		desc {Imaginary part of the constant value.}
	}
	codeblock (std) {
$refCx(output,real) $assign(output) $val(real);
$refCx(output,imag) $assign(output) $val(imag);
	}
	go {
	  addCode(std);
	}
}

