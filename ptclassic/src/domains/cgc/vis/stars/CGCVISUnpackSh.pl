defstar {
	name { VISUnpackSh }
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
Takes a single 64-bit float particle, unpacks them into four
16-bit fixed point numbers, and casts them into four float particles.
The input float particle is first separated into four 16-bit
fixed point numbers.  Once again, the order of the fixed point numbers
can be reversed.  The fixed point numbers are then up cast to float
particles.  The exponent value of each float particle can
be adjusted by the scaledown parameter.
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
	        name { scaledown }
		type { float }
		default { "1.0/32767.0" }
		desc { Output scale }
		attributes { A_CONSTANT|A_SETTABLE }
	}
	defstate {
	        name { reverse }
		type { int }
		default { FALSE }
		desc { 
TRUE unpacks with most current sample at initial position; 
FALSE unpacks with most current sample at trailing position}
		attributes { A_CONSTANT|A_SETTABLE }
	}
	code{
#define PACKOUT (4)
	}
	setup {
	  out.setSDFParams(PACKOUT,PACKOUT-1);
	}
	codeblock(globalDecl){
	  union $sharedSymbol(CGCVISUnpackSh,inoverlay){
	    vis_d64 invaluedbl;
	    vis_s16 invaluesh[4];
	  };
	}
	codeblock(mainDecl){
	  union $sharedSymbol(CGCVISUnpackSh,inoverlay) $starSymbol(packedin);
	}
	initCode{
	  addGlobal(globalDecl);
	  addDeclaration(mainDecl);
          addInclude("<vis_types.h>");
	}
	codeblock(unpackit){
	  $starSymbol(packedin).invaluedbl = (double) $ref(in);
	}
	codeblock(unpackbackwards){
	  /*scale down and unpack input*/
          $ref2(out,0)=(double) ($val(scaledown) * (double)
				 $starSymbol(packedin).invaluesh[0]);
          $ref2(out,1)=(double) ($val(scaledown) * (double)
				 $starSymbol(packedin).invaluesh[1]);
          $ref2(out,2)=(double) ($val(scaledown) * (double)
				 $starSymbol(packedin).invaluesh[2]);
          $ref2(out,3)=(double) ($val(scaledown) * (double)
				 $starSymbol(packedin).invaluesh[3]);
	}
	codeblock(unpackforwards){
	  /*scale down and unpack input*/
          $ref2(out,0)=(double) ($val(scaledown) * (double)
				 $starSymbol(packedin).invaluesh[3]);
          $ref2(out,1)=(double) ($val(scaledown) * (double)
				 $starSymbol(packedin).invaluesh[2]);
          $ref2(out,2)=(double) ($val(scaledown) * (double)
				 $starSymbol(packedin).invaluesh[1]);
          $ref2(out,3)=(double) ($val(scaledown) * (double)
				 $starSymbol(packedin).invaluesh[0]);

	}
	go {
	  addCode(unpackit);
	  if (!reverse)
	    addCode(unpackbackwards);
	  else
	    addCode(unpackforwards);
	}
}
