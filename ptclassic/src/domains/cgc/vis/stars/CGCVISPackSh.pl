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
	  const int NUMIN = 4;
	  const int UPPERBOUND = 32767;
	  const int LOWERBOUND = -32768;
	  int $starSymbol(index);
	  double $starSymbol(invalue);
	  double* $starSymbol(outvalue);
	  short* $starSymbol(packedout) = (short *)memalign(sizeof(double),sizeof(short)*NUMIN);
	}
	codeblock(packit){
	  /*scale input, check bounds of the input,*/ 
	  /*and cast each float to short*/
	      for ($starSymbol(index)=0;$starSymbol(index)<NUMIN;$starSymbol(index)++){
		$starSymbol(invalue) = (double) $val(scale) * (double) $ref2(in,$starSymbol(index));
		if ($starSymbol(invalue) <= (double) LOWERBOUND)
		  $starSymbol(packedout)[$starSymbol(index)] = (short) LOWERBOUND;
		else if ($starSymbol(invalue) >= (double) UPPERBOUND)
		  $starSymbol(packedout)[$starSymbol(index)] = (short) UPPERBOUND;
		else 
		  $starSymbol(packedout)[$starSymbol(index)] = (short) $starSymbol(invalue);
	      }	
	  /*output packed double*/	  
	  $starSymbol(outvalue) = (double *) $starSymbol(packedout);
	  $ref(out) = *$starSymbol(outvalue);
	}
	go {
	  addDeclaration(mainDecl);
	  addCode(packit);
	}
	wrapup{
	  addCode("free($starSymbol(packedout));\n");
	}
}

