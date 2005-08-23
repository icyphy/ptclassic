defstar {
    name { ConstCx }
    domain { CG56 }
    desc { Constant source }
    version { @(#)CG56ConstCx.pl	1.4 01 Oct 1996 }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 main library }
	htmldoc {
There are no runtime instructions associated with this star.
The output buffer is initialized with the specified DC value.
    }
    output {
	name {output}
	type {COMPLEX}
	attributes{P_NOINIT}
    }
    state {
	name {real}
	type {float}
	default {0.0}
	descriptor { Real part of DC value. }
    }
    state {
	name {imag}
	type {float}
	default {0.0}
	descriptor { Imaginary part of DC value. }
    }
    constructor {
	noInternalState();
    }
    
    method {
	name { initializeMemory }
	type { "void" }
	arglist { "(char memoryBank, double defaultValue)" }
	access { protected }
	code {
	    StringList out;
	    out << "	org	" << memoryBank << ":$addr(output)\n";
	    for (int i=0 ; i<output.bufSize() ; i++)
		out << "	dc	" << defaultValue << '\n';
	    addCode(out);
	}
    }

    initCode {
	initializeMemory('x',double(real));
	initializeMemory('y',double(imag));
	@	org	p:
    }
    execTime { return 0; }
}
