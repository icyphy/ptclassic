defstar {
	name { VISPackSh }
	domain { SDF }
	version { @(#)SDFVISPackSh.pl	1.8	7/9/96 }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF vis library }
	desc { 
Pack four floating point numbers into a single floating
point number.  Each input floating point number is first
down cast into a 16 bit short and then packed into a series of
four shorts.  Three things to notice:  
First assume that the input ranges from -1 to 1.
Second the code is inlined for faster performance.
Third data memory is prealigned for faster performance.
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
	hinclude {<vis_types.h>}
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
	code {
#define NUMIN (4)
	}
	protected{
	  union outoverlay {
	    vis_d64 outvaluedbl;
	    vis_s16 outvaluesh[4];
	  } packedout;
	}
        setup {
	  in.setSDFParams(NUMIN,NUMIN-1);
	}
	go {	  
	  // scale, cast, and pack input
          if (!forward) {
	    packedout.outvaluesh[0] = (short) (scale * double(in%0));
	    packedout.outvaluesh[1] = (short) (scale * double(in%1));
	    packedout.outvaluesh[2] = (short) (scale * double(in%2));
	    packedout.outvaluesh[3] = (short) (scale * double(in%3));
	  }
	  else {
	    packedout.outvaluesh[0] = (short) (scale * double(in%3));
	    packedout.outvaluesh[1] = (short) (scale * double(in%2));
	    packedout.outvaluesh[2] = (short) (scale * double(in%1));
	    packedout.outvaluesh[3] = (short) (scale * double(in%0));
	  }
	  //output packed double	  
	  out%0 << packedout.outvaluedbl;
	}
}
