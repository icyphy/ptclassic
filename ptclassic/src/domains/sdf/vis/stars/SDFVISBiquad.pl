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
        ccinclude {<vis_proto.h>, <math.h>}
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
	  name {state1}
	  type {float}
	  default {"0"}
	  desc { Internal state. }
	  attributes{ A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
	  name {state2}
	  type {float}
	  default {"0"}
	  desc { Internal state. }
	  attributes{ A_NONCONSTANT|A_NONSETTABLE }
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
	  short *dennum;
	  short *state;
	  short *outarray;
	  double *inarray;
	  double *result_filt;
	  short n0;
	  double scaledown;
	  float *result;
	}
	constructor {
	  dennum = 0;
	  state = 0;
	  outarray = 0;
	  inarray = 0;
	  result_filt = 0;
	  result = 0;
	  scaledown = 0;
	  n0 = 0;
	}
	destructor {
	  free(dennum);
	  free(state);
	  free(outarray);
	  free(inarray);
	  free(result_filt);
	  free(result);
	}
	begin {
	  int i;
	  short *indexcount;
	  double intmp,norm;

  	  // allocate tap and state arrays
	       free(dennum);
	  free(state);
	  free(inarray);
	  free(outarray);
	  free(result_filt);
	  free(result);
	  result = (float *) memalign(sizeof(float),sizeof(float));
	  dennum = (short *) memalign(sizeof(double),sizeof(short)*NUMPACK);
	  state = (short *) memalign(sizeof(float),sizeof(short)*NUMPACK);
	  inarray = (double *) memalign(sizeof(double),sizeof(double));
	  outarray = (short *) memalign(sizeof(double),sizeof(short)*NUMPACK);
	  result_filt = (double *) memalign(sizeof(double),sizeof(double)*2);

	  // find largest coefficient
	       norm = fabs(dentaps[0]) > fabs(dentaps[1]) ?
	       fabs(dentaps[0]) :fabs(dentaps[1]);
	  norm = norm > fabs(numtaps[0]) ? norm : fabs(numtaps[0]);
	  norm = norm > fabs(numtaps[1]) ? norm : fabs(numtaps[1]);
	  norm = norm > fabs(numtaps[2]) ? norm : fabs(numtaps[2]);

	  // scale down the entire transfer function
	  if(norm > 1.0)
	    scaledown = 1.0/(norm);
	  else
	    scaledown = 1.0;

	  // initialize n0
	       n0 = (short) scale*scaledown*numtaps[0];

	  // initialize denominator array
	  indexcount = dennum;
	  for(i=0;i<2;i++){
	    *indexcount++ = (short) scale*scaledown*dentaps[i];
	    *indexcount++ = (short) scale*scaledown*numtaps[i+1];
	  }

	  // initialize states
	       state[0] = 0;
	  state[1] = 0;
	  state[2] = 0;
	  state[3] = 0;
	}
	go {	
	  short *invalue;
	  short *result_den;
	  int outerloop,innerloop;
	  float *statetmp,*taps;
	  double out_dbl;
	  double *outvalue;
	  double *packedfilt;
	  double upper, lower;
	  double split_result;
	  double tmp;

	  vis_write_gsr(8);
	  *inarray = double(signalIn%0);
       	  invalue = (short *) inarray;
	  taps = (float *) dennum;
	  statetmp = (float *) state;

	  for(outerloop=3;outerloop>=0;outerloop--){

	    // find product of state and denominator/numerator
	    for(innerloop=0;innerloop<2;innerloop++){
	      upper = vis_fmuld8sux16(statetmp[innerloop],taps[innerloop]);
	      lower = vis_fmuld8ulx16(statetmp[innerloop],taps[innerloop]);
	      result_filt[innerloop] = vis_fpadd32(upper,lower);
	    }

	    split_result = vis_fpadd32(result_filt[0],result_filt[1]);
	    // find next_state
		 *result = vis_fpackfix(split_result);
	    result_den = (short *) result;
	    tmp = (double)(1/scaledown)*(invalue[outerloop] - *result_den);
	    state[2] = state[0];
	    state[3] = state[2];
	    state[0] = short(tmp);
	    state[1] = state[0];	    

	    // find output
		 out_dbl = (double)(n0*tmp/scale + *(result_den+1));
	    outarray[outerloop] = (short) out_dbl;

	  }
	  outvalue = (double *) outarray;
	  signalOut%0 <<  *outvalue;
	}
}
