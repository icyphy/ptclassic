defstar {
	name { VISAddSh }
	domain { SDF }
	version { @(#)SDFVISAddSh.pl	1.6	7/9/96 }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF vis library }
	desc { 
Add the shorts in a 16bit partitioned float to the
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
	hinclude {<vis_types.h>}
        ccinclude {<vis_proto.h>}
	go {
	  
	  /*calculate the sum*/
          out%0 << (vis_d64) vis_fpadd16(double(inA%0),double(inB%0));
      	}
}
