defstar {
	name { VISUnpackSh }
	domain { SDF }
	version { @(#)SDFVISUnpackSh.pl	1.8	7/9/96 }
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
	hinclude {<vis_types.h>}
	defstate {
	        name { scaledown }
		type { float }
		default { "1.0/32767.0" }
		desc { Output scale }
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
#define NUMOUT (4)
	}
	protected{
	  union inoverlay {
	    vis_d64 invaluedbl;
	    vis_s16 invaluesh[4];
	  } packedin;
	}
	setup {
	  out.setSDFParams(NUMOUT,NUMOUT-1);
	}
	go {
	  packedin.invaluedbl = double(in%0);

	  if (!forward) {
	    out%0 << (double) scaledown * packedin.invaluesh[0];
            out%1 << (double) scaledown * packedin.invaluesh[1];
	    out%2 << (double) scaledown * packedin.invaluesh[2];
	    out%3 << (double) scaledown * packedin.invaluesh[3];
	  }
	  else {
	    out%0 << (double) scaledown * packedin.invaluesh[3];
            out%1 << (double) scaledown * packedin.invaluesh[2];
	    out%2 << (double) scaledown * packedin.invaluesh[1];
	    out%3 << (double) scaledown * packedin.invaluesh[0];
	  }
	}
}
