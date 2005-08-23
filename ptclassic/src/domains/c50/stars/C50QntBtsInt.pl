defstar {
	name { QntBtsInt }
	domain { C50 }
	desc { Integer quantizer with adjustable offset. }
	version { $Id$ }
	author { Luis Gutierrez}
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
        explanation {
.Id "quantizer"
Normally, the \fIoutput\fR is just the two's complement number
given by the top \fInoBits\fR of the \fIinput\fR (for integer output).
        }
	input	{
		name { input }
		type { fix }
		}
        output {
		name { output }
		type { int }
	}
	state {
		name { noBits }
		type { int }
		desc {   }
		default { 4 }
	}

	constructor {
		noInternalState();
	}
        
	codeblock(std,""){
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(output)
	mar	*,ar0
	lacc	*,@noBits,ar1
	and	#ffffh,16
	sach	*,@(16-noBits)
	}

        go { 
		std();
   	}
	

	execTime { 
		return 6;
	}
}





