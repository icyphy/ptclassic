defstar {
	name { VISPackSh }
	domain { SDF }
	version { $Id$ }
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
        ccinclude { <vis_proto.h> }
	defstate {
	        name { scale }
		type { float }
		default { "32767.0" }
		desc { Input scale }
		attributes { A_CONSTANT|A_SETTABLE }
	}
	code {
#define NumIn (4)
	}
	protected{
	  short *packedout;
	}
	constructor{
	  packedout = 0;
	}
	destructor{
	  if (packedout) free(packedout);
	}
        setup {
	  in.setSDFParams(NumIn,NumIn-1);
	}
	begin {
	  if (packedout) free(packedout);
	  packedout = (short *) memalign(sizeof(double),sizeof(short)*NumIn);
        }
	go {
	  // scale, cast, and pack input
	  packedout[0] = (short) (scale * double(in%0));
	  packedout[1] = (short) (scale * double(in%1));
	  packedout[2] = (short) (scale * double(in%2));
	  packedout[3] = (short) (scale * double(in%3));

	  //output packed double	  
	  double* outvalue = (double *) packedout;
	  out%0 << *outvalue;
	}
}
