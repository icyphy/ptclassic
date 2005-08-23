defstar {
	name { QntBtsInt }
	domain { C50 }
	desc { Integer quantizer }
	version { @(#)C50QntBtsInt.pl	1.6	06 Oct 1996 }
	author { Luis Gutierrez}
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<a name="quantizer"></a>
Normally, the <i>output</i> is just the two's complement number
given by the top <i>noBits</i> of the <i>input</i> (for integer output).
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
		desc { 
the output is the input truncated to this many significant bits
		}
		default { 4 }
	}

	constructor {
		noInternalState();
	}
        
	codeblock(std,""){
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(output)
	mar	*,ar0
	lacc	*,0,ar1
	and	#65535,@(16 - int(noBits))
	sacl	*,0
	}

        go { 
		addCode(std());
   	}
	

	execTime { 
		return 6;
	}
}





