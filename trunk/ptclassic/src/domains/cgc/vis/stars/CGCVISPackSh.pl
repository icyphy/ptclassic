defstar {
	name { VISPackSh }
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
	  Pack four floating point numbers into a single floating
	  point number.  Each input floating point number is first
	  down cast into a 16 bit short and then packed into a series of
	  four shorts.  The four shorts can be packed with the either
	  the most current value leading or trailing but make sure 
	  that you pack and unpack with the same endian setting. }
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
	defstate {
	        name { forward }
		type { int }
		default { FALSE }
		desc { forward = TRUE unpacks with most current sample at
		       position 0; forward = FALSE unpacks with most
		       current sample at position 3 }
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
	  union $starSymbol(outoverlay) {
	    vis_d64 outvaluedbl;
	    vis_s16 outvaluesh[4];
	  } $starSymbol(packedout);
	}
	initCode{
	  addDeclaration(mainDecl);
	  addInclude("<vis_types.h>");
	}
	codeblock(packbackwards){
          /*scale and cast input*/
          $starSymbol(packedout).outvaluesh[0] = (short)($val(scale)*(double)$ref2(in,0));
          $starSymbol(packedout).outvaluesh[1] = (short)($val(scale)*(double)$ref2(in,1));
          $starSymbol(packedout).outvaluesh[2] = (short)($val(scale)*(double)$ref2(in,2));
          $starSymbol(packedout).outvaluesh[3] = (short)($val(scale)*(double)$ref2(in,3));

	  /*output packed double*/	  
	  $ref(out) = $starSymbol(packedout).outvaluedbl;
	}
	codeblock(packforwards){
          /*scale and cast input*/
          $starSymbol(packedout).outvaluesh[0] = (short)($val(scale)*(double)$ref2(in,3));
          $starSymbol(packedout).outvaluesh[1] = (short)($val(scale)*(double)$ref2(in,2));
          $starSymbol(packedout).outvaluesh[2] = (short)($val(scale)*(double)$ref2(in,1));
          $starSymbol(packedout).outvaluesh[3] = (short)($val(scale)*(double)$ref2(in,0));

	  /*output packed double*/	  
	  $ref(out) = $starSymbol(packedout).outvaluedbl;
	}
	go {
	  if (!forward)
	    addCode(packbackwards);
	  else
	    addCode(packforwards);
	}
}

