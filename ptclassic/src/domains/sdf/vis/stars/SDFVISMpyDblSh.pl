defstar {
	name { QuadMult16x16 }
	domain { SDF }
	version { @(#)SDFQuadMult16x16.pl	1.2 3/14/96 }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF vis library }
	desc { 
	  Multiplies the shorts in a 16bit partitioned float to the
	  corresponding shorts in a 16bit partitioned float.
	  The result is four signed shorts that is returned as
	  a single floating point number.  Each multiplication
	  results in a 32 bit result, which is then rounded to 16 bits.}

	input {
		name { InA }
		type { float }
		desc { Input float type }
	}
	input {
		name { InB }
		type { float }
		desc { Input float type }
	}
	output {
		name { Out }
		type { float }
		desc { Output float type }
	}
	go {

	  double resultu, resultl, result;

	  /*calculate the partial products*/
	  resultu = vis_fmul8sux16(InA,InB);
	  resultl = vis_fmul8ulx16(InA,InB);

	  /*calculate the final product*/
	  result = vis_fpadd16(resultu,resultl);

          Out%0 << result;
      	}
}
