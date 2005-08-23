defstar {
	name { TableInt }
	domain { CG56 }
	desc {
Implements an integer-valued lookup table.  The "values" state contains the
values to output; its first element is element zero.  An error occurs if
an out of bounds value is received.
	}
	version { @(#)CG56TableInt.pl	1.6	01 Oct 1996 }
	author { Joseph T. Buck and Brian L. Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="table lookup"></a>
	}
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	defstate {
		name { values }
		type { intarray }
		default { "-1 1" }
		desc { table of values to output }
		attributes { A_YMEM|A_CONSEC }
	}
	defstate {
		name { runTimeCheck }
		type { int }
		default { YES }
		desc {
If YES, check range of index at runtime, else omit.
This is currently not supported in the CG56 domain.
		}
	}
	constructor {
		noInternalState();
	}
	codeblock(readIdx) {
	move	#>$addr(values),r0			; table address in y memory
	move	$ref(input),n0			; table index
	nop				; account for pipelining delay
	}
	codeblock(check) {
; There is no check to make sure that the index is in bounds
	}
	codeblock(lookup) {
	move	y:(r0+n0),x0		; read table value (2 instructions)
	move	x0,$ref(output)			; write table value to output
	}
	go {
		addCode(readIdx);
		if (int(runTimeCheck)) addCode(check);
		addCode(lookup);
	}
	exectime {
		return 6;
	}
}
