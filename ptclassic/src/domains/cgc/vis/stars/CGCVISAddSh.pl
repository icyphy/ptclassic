defstar {
	name { VISAddSh }
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
	location { CGC vis library }
	desc { 
Add the four 16-bit short integers concatenated in a 64-bit float particle
to the corresponding four 16-bit short integers in a 64-bit float particle.
The result is four signed shorts that is returned as a single floating
point number.  There is no saturation arithmetic so that overflow results
in wrap around.
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
	  CGCVISBase::initCode();
	  addInclude("<vis_types.h>");
	  addInclude("<vis_proto.h>");
	}
	codeblock(addfour){
	  vis_d64 sum = vis_fpadd16((double)$ref(inA),(double)$ref(inB));
	  $ref(out) = sum;
	}
	go {
	  addCode(addfour);
      	}
}
