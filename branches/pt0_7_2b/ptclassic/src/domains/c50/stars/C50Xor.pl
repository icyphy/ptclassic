defstar {
	name { Xor }
	domain { C50 }
	desc { Exclusive or -- 2 inputs. }
	version { @(#)C50Xor.pl	1.3	01 Oct 1996 }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
This star outputs the bit-wise exclusive-or of input1 and input2.
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
	mar	*,AR5				;
	lar	AR5,#$addr(input1)		;Address input1		=> AR5
	lar	AR6,#$addr(input2)		;Address input2		=> AR6
	lar	AR7,#$addr(output)		;Address output		=> AR7
	lacc	*,0,AR6				;Accu = input1
	xor	*,AR7				;Accu = input1 XOR input2
	sacl 	*				;output = Accu
	}

	constructor {
		noInternalState();
	}

	go {
		addCode(main);
	}

	execTime {
		return 7;
	}
}
