defstar {
	name { Vis64ToFloat }
	domain { SDF }
	version { @(#)SDFVis64ToFloat.pl	1.1 3/14/96 }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF vis library }
	desc { 
	  UnPack a single floating point number into four floating 
	  point numbers.  The input floating point number is first
	  separated into four shorts and then each short is up cast 
	  to a floating point number.}
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
		default { "1.0/32767.0" }
		desc { Output scale }
		attributes { A_CONSTANT|A_SETTABLE }
	}
	code {
                #define NumOut (4)
	}
	protected{
	  double *packedin;
	}
	constructor{
	  packedin = 0;
	}
	destructor{
	  free(packedin);
       	}
	setup {
	  out.setSDFParams(NumOut,NumOut-1);
	}
	begin {
	  free(packedin);
	  packedin = (double *) memalign(sizeof(double),sizeof(double));
	}
	go {
	  
	  int index;
	  double outvalue;
	  short *invalue;
	  
	  *packedin = double(in%0);
	  invalue = (short *) packedin;
	  
	  //scale input and unpack output
	      for (index=0;index<NumOut;index++){
		outvalue = (double) scale* (double) invalue[index];
		out%(index) << outvalue;
	      }
      	}
}
