defstar {
	name { Exp }
	domain { VHDL }
	desc {
	}
	version { @(#)VHDLExp.pl	1.3 05/24/96 }
	author { Michael C. Williamson }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	codeblock (std) {
$temp(val1,float) := $ref(input);

$temp(x1,float) := $temp(val1,float);
$temp(x2,float) := $temp(val1,float) * $temp(val1,float);
$temp(x3,float) := $temp(val1,float) * $temp(val1,float) * $temp(val1,float);
$temp(x4,float) := $temp(x2,float) * $temp(x2,float);
$temp(x5,float) := $temp(x3,float) * $temp(val1,float) * $temp(val1,float);
$temp(x6,float) := $temp(x4,float) * $temp(x2,float);
$temp(x7,float) := $temp(x5,float) * $temp(val1,float) * $temp(val1,float);
$temp(x8,float) := $temp(x4,float) * $temp(x4,float);
$temp(x9,float) := $temp(x7,float) * $temp(val1,float) * $temp(val1,float);
$temp(x10,float) := $temp(x6,float) * $temp(x4,float);
$temp(x11,float) := $temp(x9,float) * $temp(val1,float) * $temp(val1,float);

$temp(term0,float) := 1.0;
$temp(term1,float) := $temp(x1,float);
$temp(term2,float) := $temp(x2,float) / 2.0;
$temp(term3,float) := $temp(x3,float) / 6.0;
$temp(term4,float) := $temp(x4,float) / 24.0;
$temp(term5,float) := $temp(x5,float) / 120.0;
$temp(term6,float) := $temp(x6,float) / 720.0;
$temp(term7,float) := $temp(x7,float) / 5040.0;
$temp(term8,float) := $temp(x8,float) / 40320.0;
$temp(term9,float) := $temp(x9,float) / 362880.0;
$temp(term10,float) := $temp(x10,float) / 3628800.0;
$temp(term11,float) := $temp(x11,float) / 39916800.0;

$temp(result,float) := 
  $temp(term0,float) + $temp(term1,float) + $temp(term2,float) +
  $temp(term3,float) + $temp(term4,float) + $temp(term5,float) +
  $temp(term6,float) + $temp(term7,float) + $temp(term8,float) +
  $temp(term9,float) + $temp(term10,float) + $temp(term11,float);

$ref(output) $assign(output) $temp(result,float);
	}
	go {
	  addCode(std);
	}
}
