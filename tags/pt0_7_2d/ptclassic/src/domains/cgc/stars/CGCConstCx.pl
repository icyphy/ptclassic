defstar {
    name {ConstCx}
    domain {CGC}
    desc { Output the complex constant. }
    version { @(#)CGCConstCx.pl	1.1 1/24/96 }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CGC main library }
    output {
	name {output}
	type {complex}
    }
    defstate {
	name {real}
	type{float}
	default {"0.0"}
	desc {Real part of DC value.}
    }
    defstate {
	name {imag}
	type{float}
	default {"0.0"}
	desc {Imaginary part of DC value.}
    }
    
    constructor { noInternalState(); }

    go {
	@	$ref(output).real = $val(real);
	@	$ref(output).imag = $val(imag);
    }

    exectime {return 0;}
}

