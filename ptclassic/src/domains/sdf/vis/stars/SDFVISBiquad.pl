defstar {
	name { QuadBiquad }
	domain { SDF }
	version { @(#)SDFQuadBiquad.pl	1.2 05/06/96 }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF vis library }
	desc { 
	  An IIR Biquad filter.
	}
	input {
	  name { signalIn }
	  type { float }
	  desc { Input float type }
	}	
	output {	
	  name { signalOut }
	  type { float }
	  desc { Output float type }
	}
        ccinclude {<vis_proto.h>}
	defstate {
	  name {numtaps}
	  type {floatarray}
	  default {
	".067455 .135 .067455"
	  }
	  desc { Filter tap numerator values (n0+n1*z^-1+n2*z^-2). }
	}		
	defstate {
	  name {dentaps}
	  type {floatarray}
	  default {
	"-1.1430 .41280"
	  }
	  desc { Filter tap denominator values (1+d1*z^-1+d2*z^-2). }
	}
	defstate {
	  name {scalefactor}
	  type {int}
	  default {"1"}
	  desc { 2^scalefactor is used to scale down the magnitude
		   of the numerator and denominator coefficients
		   between 0 and 1. }
	  attributes{ A_CONSTANT|A_SETTABLE }
	}
      	defstate {
	  name { scale }
	  type { float }
	  default { "32767.0" }
	  desc { Filter tap scale }
	  attributes { A_CONSTANT|A_SETTABLE }
	}
	code {
#define NUMPACK (4)
#define UPPERBOUND (32767) 
#define LOWERBOUND (-32768)
	}
	protected {
	  double *inarray,*split_result;
	  float *result;
	  short *dennum,*state,*outarray;
	  short n0,scaledown;
	}
	constructor {
	  inarray=split_result=0;
	  result=0;
	  dennum=state=outarray=0;
	  scaledown=n0=0;
	}
	destructor {
	  free(inarray);
	  free(split_result);
	  free(result);
	  free(dennum);
	  free(state);
	  free(outarray);
	}
	begin {
	  int i;
	  short *indexcount;

  	  // allocate tap and state arrays
	  free(inarray);
	  free(split_result);
	  free(result);
	  free(dennum);
	  free(state);
	  free(outarray);
	  inarray = (double *) memalign(sizeof(double),sizeof(double));
	  split_result = (double *)
	    memalign(sizeof(double),sizeof(short)*NUMPACK);
	  result = (float *) memalign(sizeof(float),sizeof(float));
	  dennum = (short *) memalign(sizeof(double),sizeof(short)*NUMPACK);
	  state = (short *) memalign(sizeof(float),sizeof(short)*NUMPACK);
	  outarray = (short *) memalign(sizeof(double),sizeof(short)*NUMPACK);

	  scaledown = (short) 1 << scalefactor;

	  // initialize n0
	       n0 = ((short) scale/scaledown*numtaps[0]);

	  // initialize denominator/numerator array
	       indexcount = dennum;
	  for(i=0;i<2;i++){
	    *indexcount++ = ((short) scale/scaledown*dentaps[i]);
	    *indexcount++ = ((short) scale/scaledown*numtaps[i+1]);
	  }

	  // initialize states
	       for(i=0;i<4;i++){
		 state[i]=0;
	       }
	}
	go {	
	  double *outvalue,*packedfilt;
	  double upper, lower;
	  double *allstates,*alltaps;
	  float *statemem,*result_ptr;
	  int outerloop,innerloop;
	  short *statetap_prod,*invalue,*result_den;

	  vis_write_gsr(8);
	  *inarray = double(signalIn%0);
       	  invalue = (short *) inarray;
	  statemem = (float *) state;
	  allstates = (double *) state;
	  alltaps = (double *) dennum;

	  for(outerloop=3;outerloop>=0;outerloop--){
	    // find product of states/taps
		 upper = vis_fmul8sux16(*allstates,*alltaps);
	    lower = vis_fmul8ulx16(*allstates,*alltaps);
	    *split_result = vis_fpadd16(upper,lower);
	    result_ptr = (float *) split_result;
	    *result = vis_fpadd16s(result_ptr[0],result_ptr[1]);
	    statetap_prod = (short *) result;
	    // find next_state
		 statemem[1] = statemem[0];
	    state[0]=(invalue[outerloop]-(statetap_prod[0]<<1))<<scalefactor;
	    state[1] = state[0];
	    // find output
	    outarray[outerloop]=(n0*state[0]>>15)+(statetap_prod[1]<<1);
	  }
	  outvalue = (double *) outarray;
	  signalOut%0 <<  *outvalue;
	}
}

