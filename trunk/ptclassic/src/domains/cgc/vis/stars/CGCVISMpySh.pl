defstar {
	name { VISMpySh }
	domain { CGC }
	version { $Id$ }
	author { William Chen }
	copyright {
Copyright (c) 1996-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC Visual Instruction Set library }
	desc { 
Multiply the corresponding 16-bit fixed point numbers of two
partitioned float particles.  Four signed 16-bit fixed point
numbers of a partitioned 64-bit float particle are multiplied to
those of another 64-bit float particle.  Each multiplication produces 
a 32-bit result, which is then truncated to 16 bits.  The final result 
is four 16-bit fixed point numbers that are returned as a single 
float particle.
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
	  $ref(out) = vis_fpadd16(resulthi, resultlo);
	}
	go {	  
	  addCode(multfour);
      	}
}
