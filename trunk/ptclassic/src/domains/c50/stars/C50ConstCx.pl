defstar {
    name { ConstCx }
    domain { C50 }
    desc { Constant source }
    version {@(#)C50ConstCx.pl	1.5	05/26/98}
    author { Jose Luis Pino, G. Arslan }
    copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { C50 main library }
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
	attributes {A_UMEM | A_SETTABLE | A_CONSTANT }
    }
    state {
	name {imag}
	type {float}
	default {0.0}
	descriptor { Imaginary part of DC value. }
	attributes {A_UMEM | A_SETTABLE | A_CONSTANT }
   }

    constructor {
	noInternalState();
    }
   
    execTime { return 0; }
}


