defstar {
	name { FloatToVis64 }
	domain { SDF }
	version { @(#)SDFFloatToVis64.pl	1.5 04/10/96 }
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
	  four shorts.}
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
        ccinclude {<vis_proto.h>}
	defstate {
	        name { scale }
		type { float }
		default { "32767.0" }
		desc { Input scale }
		attributes { A_CONSTANT|A_SETTABLE }
	}
	code {
                #define NumIn (4)
                #define UpperBound (32767) 
                #define LowerBound (-32768)
	}
	protected{
	  short *packedout;
	}
	constructor{
	  packedout = 0;
	}
	destructor{
	  free(packedout);
	}
        setup {
	  in.setSDFParams(NumIn,NumIn-1);
	}
	begin {
	  free(packedout);
	  packedout = (short *) memalign(sizeof(double),sizeof(short)*NumIn);
        }
	go {
	  
	  int index;
	  double invalue;
	  double *outvalue;
	  
	  //scale input, check bounds of the input, 
	      //  and cast each float to short
	      for (index=0;index<NumIn;index++){
		invalue = (double) scale * double(in%(index));
		if (invalue <= (double) LowerBound)
		  packedout[index] = (short) LowerBound;
		else if (invalue >= (double) UpperBound)
		  packedout[index] = (short) UpperBound;
		else 
		  packedout[index] = (short) invalue;
	      }	

	  /*output packed double*/	  
	  outvalue = (double *) packedout;
	  out%0 << *outvalue;
	}
}
