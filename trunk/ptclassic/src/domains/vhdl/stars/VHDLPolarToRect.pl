defstar {
	name { PolarToRect }
	domain { VHDL }
	desc { Convert magnitude and phase to rectangular form. }
	version { @(#)VHDLPolarToRect.pl	1.3 05/24/96 }
	author { Michael C. Williamson }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	input {
		name { magnitude }
		type { float }
	}
	input {
		name { phase }
		type { float }
	}
	output {
		name { x }
		type { float }
	}
	output {
		name { y }
		type { float }
	}
	codeblock (std) {
$temp(val1,float) := $ref(phase);

while $temp(val1,float) > 3.141592654 loop
  $temp(val1,float) := $temp(val1,float) - 2.0*3.141592654;
end loop;
while $temp(val1,float) < -3.141592654 loop
  $temp(val1,float) := $temp(val1,float) + 2.0*3.141592654;
end loop;


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

$temp(term1,float) := 1.0;
$temp(term2,float) := $temp(x2,float) / 2.0;
$temp(term3,float) := $temp(x4,float) / 24.0;
$temp(term4,float) := $temp(x6,float) / 720.0;
$temp(term5,float) := $temp(x8,float) / 40320.0;
$temp(term6,float) := $temp(x10,float) / 3628800.0;
$temp(term11,float) := $temp(x1,float);
$temp(term12,float) := $temp(x3,float) / 6.0;
$temp(term13,float) := $temp(x5,float) / 120.0;
$temp(term14,float) := $temp(x7,float) / 5040.0;
$temp(term15,float) := $temp(x9,float) / 362880.0;
$temp(term16,float) := $temp(x11,float) / 39916800.0;

$temp(cos,float) := $temp(term1,float) - $temp(term2,float) +
  $temp(term3,float) - $temp(term4,float) + $temp(term5,float) -
  $temp(term6,float);
$temp(sin,float) := $temp(term11,float) - $temp(term12,float) +
  $temp(term13,float) - $temp(term14,float) + $temp(term15,float) -
  $temp(term16,float);

$ref(x) $assign(x) $ref(magnitude) * $temp(cos,float);
$ref(y) $assign(y) $ref(magnitude) * $temp(sin,float);
	}
	go {
	  addCode(std);
	}
}
