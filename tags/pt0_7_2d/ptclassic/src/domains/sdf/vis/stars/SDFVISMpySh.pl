defstar {
	name { VISMpySh }
	domain { SDF }
	version { @(#)SDFVISMpySh.pl	1.2	7/9/96 }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF vis library }
	desc { 
Multiplies the shorts in a 16bit partitioned float to the
corresponding shorts in a 16bit partitioned float.
The result is four signed shorts that is returned as
a single floating point number.  Each multiplication
results in a 32 bit result, which is then rounded to 16bits.
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
	hinclude { <vis_types.h> }
        ccinclude { <vis_proto.h> }
	go {
	  vis_write_gsr(8);

	  // setup the data
	  vis_d64 dataA = double(inA%0);
	  vis_d64 dataB = double(inB%0);
	  
	  // calculate the partial products
	  vis_d64 resulthi = vis_fmul8sux16(dataA,dataB);
	  vis_d64 resultlo = vis_fmul8ulx16(dataA,dataB);
	  out%0 << vis_fpadd16(resulthi,resultlo);
      	}
}
