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
		name { In }
		type { float }
		desc { Input float type }
	}
	output {
		name { Out }
		type { float }
       		desc { Output float type }
	}
	defstate {
	        name { scale }
		type { float }
		default { "1.0" }
		desc { Output scale }
		attributes { A_CONSTANT|A_SETTABLE }
	}
	code {
                #define NumOut (4)
	}
        setup {
                Out.setSDFParams(NumOut,NumOut);
        }
	go {

	  union vis_dreg {
	    double dreg64;
	    short  sreg16[NumOut];
	  };

	  union vis_dreg packedin; 
	  double outtmp[4];

	  packedin.dreg64 = In%0;

	  /*scale output*/
	  for (int i=NumOut;i>0;i--){
	  outtmp[i-1] = double(scale)*double(packedin.sreg16[i-1]);
	  }
	  
	  /*output unpacked values*/
	  for (int i=NumOut;i>0;i--){
	  Out%(i-1) << double(outtmp[i-1]);
	  }
      	}
}
