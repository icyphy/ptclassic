defstar {
	name {Sinc}
	domain {CGC}
	desc {
This star computes the sinc of its input given in radians.
The sinc function is defined as sin(x)/x, with value 1.0 when x = 0.
	}
        version { @(#)CGCSinc.pl	1.5	10/06/96 }
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	htmldoc {
The discrete-time Fourier transform (DTFT) of a sampled sinc function is 
an ideal lowpass filter [1-2].
Modulating a sampled sinc function by a cosine function gives an
ideal bandpass signal.
This star defines the sinc function <i>without</i> using <i>pi</i>,
as is the convention in [2-3].
<h3>References</h3>
<p>[1]  
A. V. Oppenheim and R. W. Schafer, <i>Discrete-Time Signal Processing</i>,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
<p>[2]  
A. V. Oppenheim and A. Willsky, <i>Signals and Systems</i>,
Prentice-Hall: Englewood Cliffs, NJ, 1983.
<p>[3]  
R. N. Bracewell, <i>The Fourier Transform and Its Applications</i>,
McGraw-Hill: New York, 1986.
        }
	seealso { Dirichlet RaisedCos }
	input {
		name{input}
		type{float}
		desc{The input x to the sinc function.}
	}
	output {
	        name{output}
		type{float}
		desc{The output of the sinc function.}
	}
	initCode {
	        addModuleFromLibrary("ptdspSinc", "src/utils/libptdsp", "ptdsp");
	}
	go {
		addCode(sinc);
	}
	codeblock(sinc) {
	        $ref(output) = Ptdsp_Sinc((double)$ref(input));
	}
}
