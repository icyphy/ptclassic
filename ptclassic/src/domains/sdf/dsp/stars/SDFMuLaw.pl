defstar {
	name {MuLaw}
	domain {SDF}
	desc {
Transform the input using a logarithmic mapping if the "compress" is
true.  In telephony, applying the mu law to eight-bit sampled data is
called companding, and it is used to quantize the dynamic range of 
speech more accurately.  The transformation is defined in terms of
the non-negative integer parameter mu:

output = log ( 1 + mu | input | ) / log( 1 + mu ).
	}
	author { Asawaree Kalavade }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
	explanation {
.Id "Mu law compression"
.Id "compression, Mu law"
The conversion formula is
.EQ
output ~=~ log (1 + mu | input | ) / log( 1 ~+~ mu ) ~.
.EN
so this star always produces non-negative output.
.UH REFERENCES
.IP 1
Simon Haykin, \fICommunication Systems\fR, John Wiley Sons,
3rd ed., 1994, ISBN 0-471-57176-8, page 380.
	}
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}

	defstate {
		name{compress}
		type{int}
		default{"YES"}
		desc{
NO, FALSE, or 0 turns off compression.
YES, TRUE, or a non-zero integer enables compression.
		}
	}

	defstate{
		name{mu}
		type{int}
		default{255}
		desc{mu parameter, a positive integer}
	}

	defstate{
		name{denom}
		type{float}
		default{"1.0"}
		desc{denominator of the mu-law definition}
		attributes { A_NONCONSTANT|A_SETTABLE }
	}
	
	ccinclude { <math.h> }

	setup {
		if ( int(mu) < 0 ) {
			Error::abortRun(*this,
					"Value for mu must be non-negative");
			return;
		}
		denom = log(1.0 + int(mu));
	}

	go {
		double comp_value = double(input%0);

		// Compute the mu-law definition
		if (int(compress)) {
			if (comp_value < 0) comp_value = -comp_value;
			double numer = log(1.0 + int(mu) * comp_value);
			comp_value = numer / double(denom);	
		}

		output%0 << comp_value;
	}
}
