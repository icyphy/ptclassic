defstar {
	name { Xor }
	domain { CG56 }
	desc { Exclusive or -- 2 inputs. }
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
This star outputs the bit-wise exclusive-or of input1 and input2.
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



