defcore {
	name { IDCT }
	domain { ACS }
	coreCategory { FPCGC }
	corona { IDCT }
    	desc { Output the product of the inputs, as a floating-point value. }
    	version { @(#)ACSIDCTFPCGC.pl	1.2 09/08/99 }
    	author { James Lundblad }
    	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
   	}
    	location { ACS main library }
	ccinclude { <math.h> }
	setup {
		corona.input.setSDFParams(8,7);
		corona.output.setSDFParams(8,7);
	}
	codeblock(decl) {
		double $starSymbol(ONE_OVER_SQRT2), $starSymbol(W1), $starSymbol(W2), $starSymbol(W3), $starSymbol(W5), $starSymbol(W6), $starSymbol(W7);
	}
	codeblock(init) {
		$starSymbol(ONE_OVER_SQRT2) = 1.0/sqrt(2.0);
		$starSymbol(W1) = M_SQRT2*cos(M_PI/16.0);
		$starSymbol(W2) = M_SQRT2*cos(2.0*M_PI/16.0);
		$starSymbol(W3) = M_SQRT2*cos(3.0*M_PI/16.0);
		$starSymbol(W5) = M_SQRT2*cos(5.0*M_PI/16.0);
		$starSymbol(W6) = M_SQRT2*cos(6.0*M_PI/16.0);
		$starSymbol(W7) = M_SQRT2*cos(7.0*M_PI/16.0);

	}
	codeblock(main) {
		double x0, x1, x2, x3, x4, x5, x6, x7, x8;
		x1 = $ref(input,4);
		x2 = $ref(input,6);
		x3 = $ref(input,2);
		x4 = $ref(input,1);
		x5 = $ref(input,7);
		x6 = $ref(input,5);
		x7 = $ref(input,3);
		x0 = $ref(input,0);

  		/* first stage */
  		x8 = $starSymbol(W7)*(x4+x5);
  		x4 = x8 + ($starSymbol(W1)-$starSymbol(W7))*x4;
  		x5 = x8 - ($starSymbol(W1)+$starSymbol(W7))*x5;
  		x8 = $starSymbol(W3)*(x6+x7);
  		x6 = x8 - ($starSymbol(W3)-$starSymbol(W5))*x6;
  		x7 = x8 - ($starSymbol(W3)+$starSymbol(W5))*x7;
  
  		/* second stage */
  		x8 = x0 + x1;
  		x0 -= x1;
  		x1 = $starSymbol(W6)*(x3+x2);
  		x2 = x1 - ($starSymbol(W2)+$starSymbol(W6))*x2;
  		x3 = x1 + ($starSymbol(W2)-$starSymbol(W6))*x3;
  		x1 = x4 + x6;
  		x4 -= x6;
  		x6 = x5 + x7;
  		x5 -= x7;
  
  		/* third stage */
  		x7 = x8 + x3;
  		x8 -= x3;
  		x3 = x0 + x2;
  		x0 -= x2;
  		x2 = $starSymbol(ONE_OVER_SQRT2)*(x4+x5);
  		x4 = $starSymbol(ONE_OVER_SQRT2)*(x4-x5);
  
  		/* fourth stage */
  		$ref(output,0) = (x7+x1);
  		$ref(output,1) = (x3+x2);
  		$ref(output,2) = (x0+x4);
  		$ref(output,3) = (x8+x6);
  		$ref(output,4) = (x8-x6);
  		$ref(output,5) = (x0-x4);
  		$ref(output,6) = (x3-x2);
  		$ref(output,7) = (x7-x1);
	}
	initcode {
		addInclude("<math.h>");
		addDeclaration(decl);
		addCode(init);
	}
	go {
		addCode(main);
	}
	exectime {
		return(40);
	}
}
