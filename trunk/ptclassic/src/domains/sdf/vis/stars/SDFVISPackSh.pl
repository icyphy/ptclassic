defstar {
	name { FloatToVis64 }
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
	  four shorts.}
	input {
		name { In }
		type { float }
		desc { Input float type }
	}
	output {
		name { Out }
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
	  int allocflag;
	}
	constructor{
	  packedout = 0;
	  allocflag = 0;
	}
	destructor{
	  free(packedout);
	  allocflag = 0;
	}
        setup {
	  if (allocflag == 0){
	    packedout = (short *)
	      memalign(sizeof(double),sizeof(double));
	    allocflag = 1;
	  }
	  In.setSDFParams(NumIn,NumIn);
        }
	go {

	  int index;
	  double invalue;
	  double *outvalue;
	  
	  //scale input, check bounds of the input, 
	      //  and cast each float to short
	      for (index=0;index<NumIn;index++){
		invalue = (double) scale * double(In%(index));
		if (invalue <= (double) LowerBound)
		  packedout[index] = (short) LowerBound;
		else if (invalue >= (double) UpperBound)
		  packedout[index] = (short) UpperBound;
		else 
		  packedout[index] = (short) invalue;
	      }

	  /*output packed double*/	  
	  outvalue = (double *) packedout;
	  Out%0 << *outvalue;
	}
}
