defstar {
	name { PackVis64 }
	domain { CGC }
	version { $Date$ $Id$ } 
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC vis library }
	desc { 
	  Pack four floating point numbers into a single floating
	  point number.  Each input floating point number is first
	  down cast into a 16 bit short and then packed into a series of
	  four shorts.}
	input {
		name { in }
		type { float }
		desc { Input float type }
	}
	output {
		name { out }
		type { float }
		desc { Output float type }
	}
	defstate {
	        name { scale }
		type { float }
		default { "32767.0" }
		desc { Input scale }
		attributes { A_CONSTANT|A_SETTABLE }
	}
	code{
#define PACKIN (4)
	}
	constructor{
	  noInternalState();
	}
        setup {
	  in.setSDFParams(PACKIN,PACKIN-1);
	}
	codeblock(mainDecl){
	  const int $starSymbol(NUMIN) = 4;
	  const int $starSymbol(UPPERBOUND) = 32767;
	  const int $starSymbol(LOWERBOUND) = -32768;
	  short* $starSymbol(packedout) = (short *)memalign(sizeof(double),sizeof(short)*$starSymbol(NUMIN));
	}	  
	initCode{
	  addDeclaration(mainDecl);
	}
	codeblock(localDecl){
	  int index;
	  double invalue;
	  double* outvalue;
	}
	codeblock(packit){
	  /*scale input, check bounds of the input,*/ 
	  /*and cast each float to short*/
	      for (index=0;index<$starSymbol(NUMIN);index++){
		invalue = (double) $val(scale) * (double) $ref2(in,index);
		if (invalue <= (double) $starSymbol(LOWERBOUND))
		  $starSymbol(packedout)[index] = (short) $starSymbol(LOWERBOUND);
		else if (invalue >= (double) $starSymbol(UPPERBOUND))
		  $starSymbol(packedout)[index] = (short) $starSymbol(UPPERBOUND);
		else 
		  $starSymbol(packedout)[index] = (short) invalue;
	      }	
	  /*output packed double*/	  
	  outvalue = (double *) $starSymbol(packedout);
	  $ref(out) = *outvalue;
	}
	go {
	  addCode(localDecl);
	  addCode(packit);
	}
	wrapup{
	  addCode("free($starSymbol(packedout));\n");
	}
}

