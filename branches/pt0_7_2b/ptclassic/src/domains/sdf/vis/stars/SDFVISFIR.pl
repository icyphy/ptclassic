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
		default { "1.0" }
		desc { Input scale }
		attributes { A_CONSTANT|A_SETTABLE }
	}
	code {
                #define NumIn (4)
                #define UpperBound (32767) 
                #define LowerBound (-32768)
	}
        setup {
                In.setSDFParams(NumIn,NumIn);
        }
	go {
	  int i;

	  union vis_dreg {
	    double dreg64;
	    short  sreg16[NumIn];
	  };

	  union vis_dreg packedout; 
	  double intmp[NumIn];

	  /*scale input to reduce possibility of overflow*/
	  for (i=NumIn;i>0;i--){
	  intmp[i-1] = double(scale)*double(In%(i-1));
	  }
	  
	  /*check bounds of the input and cast each float to short*/
	  for (i=NumIn;i>0;i--){
	  if (intmp[i-1] <= double(LowerBound))
	    packedout.sreg16[i-1] = double(LowerBound);
	  else if (intmp[i-1] >= double(UpperBound))
	    packedout.sreg16[i-1] = double(UpperBound);
	  else 
	    packedout.sreg16[i-1] = short(intmp[i-1]);
	  }

	  /*output packed double*/	  
	  Out%0 << packedout.dreg64;
	}
}
