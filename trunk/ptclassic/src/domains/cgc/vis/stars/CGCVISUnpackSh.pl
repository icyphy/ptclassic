defstar {
	name { VISUnpackSh }
	domain { CGC }
	derivedFrom { VISBase }
	version { $Id$ }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC Visual Instruction Set library }
	desc { 
Unpack a single 64-bit floating point number into four floating point numbers.
The input floating point number is first separated into four 16-bit short
integers and then each short is up cast to a floating point number.
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
	        name { forward }
		type { int }
		default { FALSE }
		desc { forward = TRUE unpacks with most current sample at
		       position 0; forward = FALSE unpacks with most
		       current sample at position 3 }
		attributes { A_CONSTANT|A_SETTABLE }
	}
	code{
#define PACKOUT (4)
	}
	setup {
	  out.setSDFParams(PACKOUT,PACKOUT-1);
	}
	codeblock(mainDecl){
	  union $starSymbol(inoverlay) {
	    vis_d64 invaluedbl;
	    vis_s16 invaluesh[4];
	  } $starSymbol(packedin);
	}	  
	initCode{
	  CGCVISBase::initCode();
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
	  if (!forward)
	    addCode(unpackbackwards);
	  else
	    addCode(unpackforwards);
	}
}
