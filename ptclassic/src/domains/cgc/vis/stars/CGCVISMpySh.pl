defstar {
	name { VISMpySh }
	domain { CGC }
	version { $Id$ }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC vis library }
	desc { 
	  Multiplies the shorts in a 16bit partitioned float to the
	  corresponding shorts in a 16bit partitioned float.
	  The result is four signed shorts that is returned as
	  a single floating point number.  Each multiplication
	  results in a 32 bit result, which is then rounded to 16bits.
	    }
	input {
	  name { inA }
	  type { float }
	  desc { Input float type }
	}
	input {
	  name { inB }
	  type { float }
	  desc { Input float type }
	}
	output {
	  name { out }
	  type { float }
	  desc { Output float type }
	}
	constructor {
	  noInternalState();
	}
	initCode{
	  addInclude("<vis_proto.h>");
	  addInclude("<vis_types.h>");
	}
	codeblock(multfour){
	  /* setup the data */
	  vis_d64 dataA = (double) $ref(inA);
	  vis_d64 dataB = (double) $ref(inB);
	  
	  /* calculate the partial products */
	  vis_d64 resulthi = vis_fmul8sux16(dataA,dataB);
	  vis_d64 resultlo = vis_fmul8ulx16(dataA,dataB);
	  $ref(out) = vis_fpadd16(resulthi,resultlo);
	}
	go {	  
	  addCode(multfour);
      	}
}
