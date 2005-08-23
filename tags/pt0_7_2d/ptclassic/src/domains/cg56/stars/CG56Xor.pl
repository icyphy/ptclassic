defstar {
	name { Xor }
	domain { CG56 }
	desc { Exclusive or -- 2 inputs. }
	version { @(#)CG56Xor.pl	1.9 06 Oct 1996 }
	author { Chih-Tsung Huang }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
This star outputs the bitwise exclusive-or of <i>input1</i> and <i>input2</i>.
	}
	execTime {
		return 4;
	}
	input {
		name {input1}
		type {anytype}
	}
	input {
		name {input2}
		type {=input1}
	}
	output {
		name {output}
		type {=input1}
	}
	constructor {
		noInternalState();
	}
	codeblock (main) {
; read inputs --> x1,a1
	clr 	a		$ref(input1),x1
	move	$ref(input2),a1
; perform exclusive or
	eor	x1,a
	move 	a1,$ref(output)
	}
	go {
		addCode(main);
	}
}



