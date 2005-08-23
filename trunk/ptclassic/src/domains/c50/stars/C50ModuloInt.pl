defstar {
	name {ModuloInt}
	domain {C50}
	desc { 
The output is equal to the remainder after dividing the
integer input by the integer "modulo" parameter.  Currently only
moduli	which are powers of two are supported.
	}
	version { @(#)C50ModuloInt.pl	1.2  7/10/96  }
	author { Luis Gutierrez, based on CG56 version }
	acknowledge { Brian L. Evans, author of the CG56 version }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	input {
		name{input}
		type{int}
	}
	output {
		name{output}
		type{int}
	}
	defstate {
		name{modulo}
		type{int}
		default{"2"}
		desc {The modulo parameter}
	}

	codeblock(powerOfTwo,"") {
	lar	ar0,#$addr(input)
	mar	*,ar0
	lacc	*,16,ar1
	and	#65535,@(powOfTwo)		
	lar	ar1,#$addr(output)
	sach	*,0
	}

	protected{
		int powOfTwo;
	}

	setup {
		if ( int(modulo) <= 0 ) {
		    Error::abortRun(*this,
				    "The modulo parameter must be positive");
		    return;
		}
		powOfTwo = 0;
		int  temp = 1;
		for (int i = 0; i<= 16; i++, powOfTwo++){
			if (int(modulo)<= temp) break;
			temp = 2*temp;
		}

	}
	go {
		addCode(powerOfTwo());
	}
	exectime{
		return 6;
	}
}








