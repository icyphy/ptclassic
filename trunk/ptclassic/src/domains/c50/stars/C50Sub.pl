defstar {
	name { Sub }
	domain { C50 }
	desc { Input subtractor }
	version { $Id$ }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 arithmetic library }
	explanation {
.PP
Output the \fIpos\fR minus all \fIneg\fR inputs.
        }    
	input {
		name { pos }
		type { fix }
 	}
	inmulti {
		name { neg }
		type { fix }
 	}
	output {
		name { output }
		type { fix }
	}
        codeblock(main) {
	mar	*,AR0
	lar	AR0,#$addr(pos)			;Address pos		=>AR0
        lacc    *,15,AR1			;Accu	= pos
        }
        codeblock(loop, "int i") {
	lar	AR1,#$addr(neg#@i)		;Address neg#i		=>AR1
        sub	*,15				;Accu pos-neg#i
        }
        codeblock(done) {
	mar	*,AR7
	lar	AR7,#$addr(output)		;Address output		=>AR7
        sach    *,1				;output = Accu
        }

	go {
		addCode(main);
		for (int i = 1; i <= neg.numberPorts(); i++) {
			addCode(loop(i));
		}
		addCode(done);
	}
	execTime {
		return 2 * int(neg.numberPorts()) + 6;
	}
}
