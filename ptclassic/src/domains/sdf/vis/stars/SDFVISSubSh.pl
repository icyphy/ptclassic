defstar {
	name { QuadSub16 }
	domain { SDF }
	version { $Date$ $Id$}
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF vis library }
	desc { 
	  Subtract the shorts in a 16bit partitioned float to the
	  corresponding shorts in a 16bit partitioned float.
	  The result is four signed shorts that is returned as
	  a single floating point number.  There is no saturation
	  arithmetic so that overflow results in wraparound. }

	input {
		name { InA }
		type { float }
		desc { Input float type }
	}
	input {
		name { InB }
		type { float }
		desc { Input float type }
	}
	output {
		name { Out }
		type { float }
		desc { Output float type }
	}
        ccinclude {<vis_proto.h>}
	go {

	  double diff =0.0;
	  
	  /*calculate the difference*/
	  diff = vis_fpsub16(double(InA%0),double(InB%0));

          Out%0 << diff;
      	}
}

