defstar {
	name { VISUnpackSh }
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
UnPack a single floating point number into four floating 
point numbers.  The input floating point number is first
separated into four shorts and then each short is up cast 
to a floating point number. Three things to notice:  
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
	ccinclude {<vis_proto.h>}
	defstate {
	        name { scaledown }
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
	  if (packedin) free(packedin);
       	}
	setup {
	  out.setSDFParams(NumOut,NumOut-1);
	}
	begin {
	  if (packedin) free(packedin);
	  packedin = (double *) memalign(sizeof(double),sizeof(double));
	}
	go {
	  *packedin = double(in%0);
	  short* invalue = (short *) packedin;

	  // scale down and unpack input
	  out%0 << (double) (scaledown * (double) invalue[0]);
	  out%1 << (double) (scaledown * (double) invalue[1]);
	  out%2 << (double) (scaledown * (double) invalue[2]);
	  out%3 << (double) (scaledown * (double) invalue[3]);
	}
}
