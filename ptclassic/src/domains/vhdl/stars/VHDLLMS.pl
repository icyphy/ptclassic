defstar {
	name { LMS }
	domain { VHDL }
	desc { 
Adaptive filter using LMS adaptation algorithm. 
}
	version { $Id$ }
	author { E. A. Lee, A. P. Kalavade, M. C. Williamson }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	input {
		name { signalIn }
		type { int }
	}
	input {
                name { error }
                type { int }
        }
	output {
		name { signalOut }
		type { int }
	}
	defstate {
		name { delay1 }
		type { int }
		default { 0 }
		desc { Input stream delayed by 1. }
	}
	defstate {
		name { delay2 }
		type { int }
		default { 0 }
		desc { Input stream delayed by 2. }
	}
	defstate {
		name { delay3 }
		type { int }
		default { 0 }
		desc { Input stream delayed by 3. }
	}
	defstate {
		name { tap0 }
		type { int }
		default { 1 }
		desc { filter tap 0 }
	}
	defstate {
		name { tap1 }
		type { int }
		default { 1 }
		desc { filter tap 1 }
	}
	defstate {
		name { tap2 }
		type { int }
		default { 1 }
		desc { filter tap 2 }
	}
	defstate {
		name { tap3 }
		type { int }
		default { 1 }
		desc { filter tap 3 }
	}
        defstate {
                name { stepSize }
                type { float }
                default { "0.01" }
                desc { Adaptation step size. }
        }
	codeblock(std)
	{
$ref(signalOut) $assign(signalOut) $ref(signalIn) * $ref(tap0) + $ref(delay1) *
  $ref(tap1) + $ref(delay2) * $ref(tap2) + $ref(delay3) * $ref(tap3);
$ref(delay3) $assign(delay3) $ref(delay2);
$ref(delay2) $assign(delay2) $ref(delay1);
$ref(delay1) $assign(delay1) $ref(signalIn);
$ref(tap0) $assign(tap0) $ref(tap0) + $val(stepSize) * $ref(error) * $ref(signalIn);
$ref(tap1) $assign(tap1) $ref(tap1) + $val(stepSize) * $ref(error) * $ref(delay1);
$ref(tap2) $assign(tap2) $ref(tap2) + $val(stepSize) * $ref(error) * $ref(delay2);
$ref(tap3) $assign(tap3) $ref(tap3) + $val(stepSize) * $ref(error) * $ref(delay3);
	}
	go {
	  addCode(std);
	}
}
