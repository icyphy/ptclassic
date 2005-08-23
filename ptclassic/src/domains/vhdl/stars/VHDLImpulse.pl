defstar {
	name { Impulse }
	domain { VHDL }
	desc {
Generate a stream of impulses of size "level" (default 1.0).
The period is given by "period" (default 0).
If period = 0 then only one impulse is generated.
	}
	version { @(#)VHDLImpulse.pl	1.2 03/07/96 }
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
		type { float }
	}
	defstate {
		name { level }
		type { float }
		default { 1.0 }
		desc { The height of the impulse. }
	}
	defstate {
		name { period }
		type { int }
		default { 0 }
		desc { The period of the impulse train, 0 = aperiodic. }
	}
	defstate {
		name { count }
		type { int }
		default { 0 }
		desc { An internal state. }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	codeblock (std) {
if $ref(count) = 0
  $ref(output) $assign(output) $val(value);
else
  $ref(output) $assign(output) 0;
end if;
$ref(count) $assign(count) $ref(count) + 1;
	}
	codeblock (periodic) {
if $ref(count) >= $val(period)
  $ref(count) $assign(count) 0;
end if;
	}
	go {
	  addCode(std);
	  if (period == 0) {
	    addCode(periodic);
	  }
	}
}

