ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 5/29/90
 Converted to use preprocessor, 10/3/90 

 Computes product of n inputs.

**************************************************************************/
}

defstar {
	name {FloatProduct}
	domain {SDF}
	desc {
		"Output the product of the inputs, as a float value"
	}
	inmulti {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {float}
	}
	start { input.reset();}
	go {
		double product = 1.0;
		for(int i=input.numberPorts(); i>0; i--)
			product *= float(input()%0);
		output%0 << product;
	}
}
