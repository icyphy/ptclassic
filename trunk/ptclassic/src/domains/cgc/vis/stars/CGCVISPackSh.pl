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
	  short* $starSymbol(packedout) =
	    (short*)memalign(sizeof(double),sizeof(short)*4);
	}	  
	initCode{
	  addDeclaration(mainDecl);
	}
	codeblock(localDecl){
	  double* outvalue;
	}
	codeblock(packit){
          /*scale,cast,and pack input*/
          $starSymbol(packedout)[0] = (short)
	    ($val(scale)*(double)$ref2(in,0));
          $starSymbol(packedout)[1] = (short)
	    ($val(scale)*(double)$ref2(in,1));
          $starSymbol(packedout)[2] = (short)
	    ($val(scale)*(double)$ref2(in,2));
          $starSymbol(packedout)[3] = (short)
	    ($val(scale)*(double)$ref2(in,3));
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

