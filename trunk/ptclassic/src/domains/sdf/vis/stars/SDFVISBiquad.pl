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
        ccinclude {<vis_proto.h>, <math.h>, <stdio.h>}
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
#define TWO (2)
#define THREE (3)
#define NUMPACK (4)
#define UPPERBOUND (32767) 
#define LOWERBOUND (-32768)
	}
	protected {
	  short *numerator;
	  short *denominator;
	  short *state;
	  short *outarray;
	  double *inarray;
	  float *result_filt;
	  short n0,d0;
	  double scaledown,s1,s2;
	  float *result;
	}
	constructor {
	  numerator = 0;
	  denominator = 0;
	  state = 0;
	  outarray = 0;
	  inarray = 0;
	  result_filt = 0;
	  result = 0;
	  scaledown = s1 = s2 = 0;
	  n0 = d0 = 0;
	}
	destructor {
	  free(numerator);
	  free(denominator);
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
	       free(numerator);
	  free(denominator);
	  free(state);
	  free(inarray);
	  free(outarray);
	  free(result_filt);
	  free(result);
	  result = (float *) memalign(sizeof(float),sizeof(float));
          numerator = (short *) memalign(sizeof(float),sizeof(short)*TWO);
          denominator = (short *) memalign(sizeof(float),sizeof(short)*TWO);
	  state = (short *) memalign(sizeof(float),sizeof(short)*TWO);
	  inarray = (double *) memalign(sizeof(double),sizeof(double));
	  outarray = (short *) memalign(sizeof(double),sizeof(short)*NUMPACK);
	  result_filt = (float *) memalign(sizeof(double),sizeof(float)*TWO);

	  // find largest coefficient
	  norm = fabs(dentaps[0]) > fabs(dentaps[1]) ? fabs(dentaps[0]) :fabs(dentaps[1]);
	  norm = norm > fabs(numtaps[0]) ? norm : fabs(numtaps[0]);
	  norm = norm > fabs(numtaps[1]) ? norm : fabs(numtaps[1]);
	  norm = norm > fabs(numtaps[2]) ? norm : fabs(numtaps[2]);

	  // scale down the entire transfer function
	  if(norm > 1.0)
	    scaledown = 1.0/(norm);
	  else
	    scaledown = 1.0;

	  // initialize d0 and n0
	       d0 = (short) (scale*scaledown);
	  intmp = scale*scaledown*numtaps[0];
	  if (intmp <= (double)(LOWERBOUND)){
	    n0 = (short)(LOWERBOUND);
	  }
	  else if (intmp >= (double)(UPPERBOUND)){
	    n0 = (short)(UPPERBOUND);
	  }
	  else{ 
	    n0 = (short)(intmp);
	  }

	  // initialize denominator array
	  indexcount = denominator;
	  for(i=0;i<2;i++){
	    intmp = scale*scaledown*dentaps[i];
	    if (intmp <= (double)(LOWERBOUND)){
	      *indexcount++ = (short)(LOWERBOUND);
	    }
	    else if (intmp >= (double)(UPPERBOUND)){
	      *indexcount++ = (short)(UPPERBOUND);
	    }
	    else{ 
	      *indexcount++ = (short)(intmp);
	    }
	  }

	  // initialize num array
	  indexcount = numerator;
	  for(i=0;i<2;i++){
	    intmp = scale*scaledown*numtaps[i+1];
	    if (intmp <= (double)(LOWERBOUND)){
	      *indexcount++ = (short)(LOWERBOUND);
	    }
	    else if (intmp >= (double)(UPPERBOUND)){
	      *indexcount++ = (short)(UPPERBOUND);
	    }
	    else{ 
	      *indexcount++ = (short)(intmp);
	    }

	  }
	  s1 = double(state1);
	  s2 = double(state2);
	}
	go {	
	  short *invalue;
	  short next_state_sh;
	  int numloop;
	  double next_state_dbl,out_dbl;
	  double *outvalue;
	  double *packedfilt;
	  double upper, lower;
	  double split_result;
	  float splithi, splitlo;
	  float *statetmp,*numtmp,*dentmp;
	  short *result_den;
	  short *result_num;

	  vis_write_gsr(8);
	  *inarray = double(signalIn%0);
       	  invalue = (short *) inarray;
	  dentmp = (float *) denominator;
	  numtmp = (float *) numerator;

	  for(numloop=3;numloop>=0;numloop--){
	    // initialize state array
		 state[0] = (short) s1;
	    state[1] = (short) s2;
	    statetmp = (float *) state;

	    // find product of state and denominator
		upper = vis_fmuld8sux16(*statetmp,*dentmp);
	    lower = vis_fmuld8ulx16(*statetmp,*dentmp);
	    split_result = vis_fpadd32(upper,lower);
            splithi = vis_read_hi(split_result);
            splitlo = vis_read_lo(split_result);
	    result_filt[0] = vis_fpadd32s(splithi,splitlo);

	    // find product of state and numerator
		 upper = vis_fmuld8sux16(*statetmp,*numtmp);
	    lower = vis_fmuld8ulx16(*statetmp,*numtmp);
	    split_result = vis_fpadd32(upper,lower);
            splithi = vis_read_hi(split_result);
            splitlo = vis_read_lo(split_result);
	    result_filt[1] = vis_fpadd32s(splithi,splitlo);

	    // find next_state
		 packedfilt = (double *) result_filt;
	    *result = vis_fpackfix(*packedfilt);
	    result_den = (short *) result;
	    result_num = (result_den +1);
	    next_state_dbl = (double)(1/scaledown)*(invalue[numloop] - *result_den);
	    if (next_state_dbl <= (double)(LOWERBOUND)){
	      next_state_sh = (short)(LOWERBOUND);
	    }
	    else if (next_state_dbl >= (double)(UPPERBOUND)){
	      next_state_sh = (short)(UPPERBOUND);
	    }
	    else{ 
	      next_state_sh = (short) next_state_dbl;
	    }
	    out_dbl = (double)(n0*next_state_sh/scale + *result_num);
	    if (out_dbl <= (double)(LOWERBOUND)){
	      outarray[numloop] = (short)(LOWERBOUND);
	    }
	    else if (out_dbl >= (double)(UPPERBOUND)){
	      outarray[numloop] = (short)(UPPERBOUND);
	    }
	    else{ 
	      outarray[numloop] = (short) out_dbl;
	    }
	    // update states
		 s2 = (double)s1;
	    s1 = (double) next_state_sh;
	  }
	  outvalue = (double *) outarray;
	  signalOut%0 <<  *outvalue;
	}
}
