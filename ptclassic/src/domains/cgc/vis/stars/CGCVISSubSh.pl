defstar {
	name { QuadSub16 }
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
	  Subtract the shorts in a 16bit partitioned float to the
	  corresponding shorts in a 16bit partitioned float.
	  The result is four signed shorts that is returned as
	  a single floating point number.  There is no saturation
	  arithmetic so that overflow results in wraparound. 
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
	  addInclude("<vis_proto.h>");
	}
	codeblock(localDecl){
          double diff=0.0;
        }
	codeblock(subfour){
	  diff = vis_fpsub16((double)$ref(inA),(double)$ref(inB));
	  $ref(out) = diff;
	}
	go {
	  addCode(localDecl);
	  addCode(subfour);
      	}
}
