defstar {
	name { QuadBiquad }
	domain { CGC }
	version { $Id$ }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC vis library }
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
	codeblock(mainDecl) {
	  const int $starSymbol(NUMPACK)=4;
	  double* $starSymbol(inarray) = (double*)
	    memalign(sizeof(double),sizeof(double));
	  double* $starSymbol(split_result) = (double*)
	    memalign(sizeof(double),sizeof(short)*$starSymbol(NUMPACK));
	  float* $starSymbol(result) = (float *)
	    memalign(sizeof(float),sizeof(float));
	  short* $starSymbol(dennum) = (short *)
	    memalign(sizeof(double),sizeof(short)*$starSymbol(NUMPACK));
	  short* $starSymbol(state) = (short *)
	    memalign(sizeof(float),sizeof(short)*$starSymbol(NUMPACK));
	  short* $starSymbol(outarray) = (short *)
	    memalign(sizeof(double),sizeof(short)*$starSymbol(NUMPACK));
	}
	initCode {
	  addInclude("<vis_proto.h>");
	  addDeclaration(mainDecl);
	}
	codeblock(localDecl) {
	  double *outvalue,*packedfilt;
	  double upper, lower;
	  double *allstates,*alltaps;
	  float *statemem,*result_ptr;
	  int outerloop,innerloop,i;
	  short *statetap_prod,*invalue,*result_den;
	  short *indexcount,n0,scaledown;
	}
	codeblock(filter){
	  scaledown = (short) 1 << $val(scalefactor);
	  /* initialize n0 */
	  n0 = ((short) $val(scale)/scaledown*(double)$ref2(numtaps,0));
	  /*initialize denominator/numerator array*/
	  indexcount = $starSymbol(dennum);
	  for(i=0;i<2;i++){
	    *indexcount++ = (short)
	      $val(scale)/scaledown*(double)$ref2(dentaps,i);
	    *indexcount++ = (short)
	      $val(scale)/scaledown*(double)$ref2(numtaps,i+1);
	  }
	  *$starSymbol(inarray) = (double)$ref(signalIn);
       	  invalue = (short *) $starSymbol(inarray);
	  statemem = (float *) $starSymbol(state);
	  allstates = (double *) $starSymbol(state);
	  alltaps = (double *) $starSymbol(dennum);

	  for(outerloop=3;outerloop>=0;outerloop--){
	    /* find product of states/taps */
	    upper = vis_fmul8sux16(*allstates,*alltaps);
	    lower = vis_fmul8ulx16(*allstates,*alltaps);
	    *$starSymbol(split_result) = vis_fpadd16(upper,lower);
	    result_ptr = (float *) $starSymbol(split_result);
	    *$starSymbol(result) = vis_fpadd16s(result_ptr[0],result_ptr[1]);
	    statetap_prod = (short *) $starSymbol(result);
	    /* find next_state */
	    statemem[1] = statemem[0];
	    $starSymbol(state)[0]=
	      (invalue[outerloop]-(statetap_prod[0]<<1))<<$val(scalefactor);
	    $starSymbol(state)[1] = $starSymbol(state)[0];
	    /* find output */
	    $starSymbol(outarray)[outerloop] =
	      (n0*$starSymbol(state)[0]>>15)+(statetap_prod[1]<<1);
	  }
	  outvalue = (double *) $starSymbol(outarray);
	  $ref(signalOut)= *outvalue;
	}	
	go {
	  addCode(localDecl);
	  addCode(filter);
	}
	codeblock(freeit){
          free($starSymbol(inarray));
          free($starSymbol(split_result));
          free($starSymbol(result));
          free($starSymbol(dennum));
          free($starSymbol(state));
          free($starSymbol(outarray));
	}
	wrapup {
          addCode(freeit);
        }
}

