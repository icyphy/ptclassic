defstar {
	name { UnpackVis64 }
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
	  UnPack a single floating point number into four floating 
	  point numbers.  The input floating point number is first
	  separated into four shorts and then each short is up cast 
	  to a floating point number.}
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
		default { "1.0/32767.0" }
		desc { Output scale }
		attributes { A_CONSTANT|A_SETTABLE }
	}
	code{
#define PACKOUT (4)
	}
	setup {
	  out.setSDFParams(PACKOUT,PACKOUT-1);
	}
	codeblock(mainDecl){
	  const int $starSymbol(NUMOUT) = 4;
          double *$starSymbol(packedin) = (double *)memalign(sizeof(double),sizeof(double));
	}
	initCode{
	  addDeclaration(mainDecl);
	}
	codeblock(localDecl){
	  int index;
	  double outvalue;
	  short *invalue;
	}
	codeblock(unpackit){
	  *$starSymbol(packedin) = (double) $ref(in);
	  invalue = (short *) $starSymbol(packedin);
	  
	  /*scale input and unpack output*/
	      for (index=0;index<$starSymbol(NUMOUT);index++){
		outvalue = (double) $val(scale)* (double) invalue[index];
		$ref2(out,index) = outvalue;
	      }	
      	}
	go {
	  addCode(localDecl);
	  addCode(unpackit);
	}
	wrapup{
	  addCode("free($starSymbol(packedin));\n");
       	}
}
