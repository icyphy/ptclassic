ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 5/29/90
 Converted to use preprocessor, 9/26/90 

 Computes complex product of n inputs.

**************************************************************************/
}

defstar {
	name {ComplexProduct}
	domain {SDF}
	desc {
		"Output the product of the inputs, as a complex value"
	}
	inmulti {
		name {input}
		type {complex}
	}
	output {
		name {output}
		type {complex}
	}
	start { input.reset();}
	go {
		Complex product = 1.0;
		for(int i=input.numberPorts(); i>0; i--)
			product *= Complex(input()%0);
		output%0 << product;
	}
}