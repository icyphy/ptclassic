defstar {
	name { VISPackSh }
	domain { CGC }
	version { $Id$ } 
	author { William Chen }
	copyright {
Copyright (c) 1996-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC Visual Instruction Set library }
	desc { 
Takes four float particles, casts them into four signed 16-bit
fixed point numbers, and packs them into a single 64-bit float
particle.  The input float particles are first down cast 
into 16-bit fixed point numbers.  The location of the binary point 
of the fixed point number can be placed anywhere by adjusting 
the scale parameter.  The fixed point numbers are then concatenated
to produce a 64-bit result.  The order of the fixed point numbers
can be reversed so that the most current input either leads or trails the pack,
ie reverse equals FALSE produces (x[n],x[n-1],x[n-2],x[n-3]) and reverse equals 
TRUE produces (x[n-3],x[n-2],x[n-1],x[n]).
        }
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
	        name { reverse }
		type { int }
		default { FALSE }
		desc {
TRUE packs with most current sample at inital position;
FALSE packs with most current sample at trailing position}
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
	codeblock(globalDecl){
	  union $sharedSymbol(CGCVISPackSh,outoverlay) {
	    vis_d64 outvaluedbl;
	    vis_s16 outvaluesh[4];
	  };
	}
	codeblock(mainDecl){
	  union $sharedSymbol(CGCVISPackSh,outoverlay) $starSymbol(packedout);
	}
	initCode{
	  addGlobal(globalDecl);
	  addDeclaration(mainDecl);
	  addInclude("<vis_types.h>");
	}
	codeblock(packbackwards){
          /*scale and cast input*/
          $starSymbol(packedout).outvaluesh[0] =
	    (short)($val(scale)*(double)$ref2(in,0));
          $starSymbol(packedout).outvaluesh[1] =
	    (short)($val(scale)*(double)$ref2(in,1));
          $starSymbol(packedout).outvaluesh[2] =
	    (short)($val(scale)*(double)$ref2(in,2));
          $starSymbol(packedout).outvaluesh[3] =
	    (short)($val(scale)*(double)$ref2(in,3));

	  /*output packed double*/	  
	  $ref(out) = $starSymbol(packedout).outvaluedbl;
	}
	codeblock(packforwards){
          /*scale and cast input*/
          $starSymbol(packedout).outvaluesh[0] =
	    (short)($val(scale)*(double)$ref2(in,3));
          $starSymbol(packedout).outvaluesh[1] =
	    (short)($val(scale)*(double)$ref2(in,2));
          $starSymbol(packedout).outvaluesh[2] =
	    (short)($val(scale)*(double)$ref2(in,1));
          $starSymbol(packedout).outvaluesh[3] =
	    (short)($val(scale)*(double)$ref2(in,0));

	  /*output packed double*/	  
	  $ref(out) = $starSymbol(packedout).outvaluedbl;
	}
	go {
	  if (!reverse)
	    addCode(packbackwards);
	  else
	    addCode(packforwards);
	}
}
