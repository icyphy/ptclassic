defstar {
	name { VISAddSh }
	domain { CGC }
	version { $Id$ }
	author { William Chen }
	copyright {
Copyright (c) 1996-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC vis library }
	desc { 
Add the corresponding 16-bit fixed point numbers of two
partitioned float particles.  Four signed 16-bit fixed point
numbers of a partitioned 64-bit float particle are added to those
of another 64-bit float particle.  The result is returned as a 
single 64-bit float particle.  There is no saturation arithmetic 
so that overflow results in wrap around.
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
