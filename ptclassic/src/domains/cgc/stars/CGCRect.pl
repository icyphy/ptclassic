defstar {
	name { Rect }
	domain { CGC }
	desc {
Generates a rectangular pulse of height "height" (default 1.0).
and width "width" (default 8).  If "period" is greater than zero,
then the pulse is repeated with the given period.
	}
	version {$Id$}
	author { Kennard White }
	acknowledge { SDF version by J. T. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
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
		desc { If greater than zero, the period of the pulse stream. }
	}
	defstate {
		name { count }
		type { int }
		default { 0 }
		desc { Internal counting state. }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	codeblock(cbMain) {
		$ref(output) = ($ref(count) < $val(width)) ? $val(height) : 0.0;
		++$ref(count);
		if ( $val(period) > 0 && $ref(count) >= $val(period) )
		    $ref(count) = 0;
	}
	go {
		addCode(cbMain);
	}
}
