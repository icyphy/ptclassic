defstar {
	name { Rect }
	domain { VHDL }
	desc {
Generate a rectangular pulse of height "height" (default 1.0).
and width "width" (default 8).  If "period" is greater than zero,
then the pulse is repeated with the given period.
	}
	version { @(#)VHDLRect.pl	1.2 03/07/96 }
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
		name { height }
		type { float }
		default { 1.0 }
		desc { Height of the rectangular pulse. }
	}
	defstate {
		name { width }
		type { int }
		default { 8 }
		desc { Width of the rectangular pulse. }
	}
	defstate {
		name { period }
		type { int }
		default { 0 }
		desc { The period of the pulse stream, 0 = aperiodic. }
	}
	defstate {
		name { count }
		type { int }
		default { 0 }
		desc { Internal counting state. }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	codeblock (std) {
if $ref(count) < $val(width)
  $ref(output) $assign(output) $val(height);
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
