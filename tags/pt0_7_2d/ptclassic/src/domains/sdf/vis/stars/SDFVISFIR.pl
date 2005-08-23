defstar {
	name { VISFIR }
	domain { SDF }
	version { @(#)SDFVISFIR.pl	1.11	7/9/96 }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF vis library }
	desc { 
A finite impulse response (FIR) filter.
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
	hinclude {<vis_types.h>}
        ccinclude {<vis_proto.h>}
	defstate {
	  name {taps}
	  type {floatarray}
	  default {
	"-.040609 -.001628 .17853 .37665 .37665 .17853 -.001628 -.040609"
	  }
	  desc { Filter tap values. }
	}		
      	defstate {
	  name { scale }
	  type { float }
	  default { "32767.0" }
	  desc { Filter tap scale }
	  attributes { A_CONSTANT|A_SETTABLE }
	}
	defstate {
          name {scalefactor}
          type {int}
          default {"0"}
          desc { 2^scalefactor is used to scale down the magnitude
                   of the numerator coefficients between 0 and 1. }
          attributes{ A_CONSTANT|A_SETTABLE }
        }
	code {
#define NUMIN (1)
#define	NUMPACK (4)
	}
	protected {
	  int taplength,tappadlength,maxpast,numloop;
	  short *shift_taparray,scaledown;
	}
	constructor {
	  shift_taparray=0;
	}
	destructor {
	  free(shift_taparray);
	}
        setup {

	  // determine tap length
	  taplength = taps.size();
	  
	  // determine input buffer size
	  if (taplength == 0)
	    maxpast = 1;
	  else if ((taplength-1)%NUMPACK==0)
	    maxpast = (int)(taplength-1)/NUMPACK+1;
	  else
	    maxpast = (int)(taplength-1)/NUMPACK+2;
	  tappadlength = NUMPACK*maxpast;
	  signalIn.setSDFParams(NUMIN,maxpast-1);
	}
	begin {
	  int taprowindex, tapcolindex;
	  vis_s16 *indexcount;

  	  // allocate shifted tap and tmp arrays
	  if (shift_taparray) free(shift_taparray);
	  shift_taparray = (short *)
	    memalign(sizeof(double),sizeof(short)*NUMPACK*tappadlength);

          scaledown = (short) 1 << scalefactor;

	  // initialize shifted taparrays to zero
	  indexcount = shift_taparray;
	  for(taprowindex=0;taprowindex<NUMPACK;taprowindex++){
	    for(tapcolindex=0;tapcolindex<tappadlength;tapcolindex++){
	      *indexcount++ = 0;
	    }
	  }
	  
	  // fill taparrays
	  for(taprowindex=0;taprowindex<NUMPACK;taprowindex++){
	    indexcount = shift_taparray + (tappadlength+1)*taprowindex;
	    for(tapcolindex=0;tapcolindex<taplength;tapcolindex++){
	      // scale taps, check under/overflow, and cast to short 
	      *indexcount++ = (short)(scale/scaledown*taps[tapcolindex]);
	    }
	  }

	  // loop once for each set of filter taps
	  if (taplength == 0)
	    numloop = 0;	
	  else		
	    numloop = maxpast;
	}
	go {	
	  vis_d64 *tapptr0,*tapptr1,*tapptr2,*tapptr3,tapvalue;
	  vis_d64 pairlohi,pairlolo,pairhilo,pairhihi,dhi,dlo;
	  vis_d64 pairlo,pairhi,pair,*packedaccum;
	  vis_d64 accumpair0,accumpair1,accumpair2,accumpair3;
	  vis_f32  packedhi,packedlo,datahi,datalo,r0,r1,r2,r3;
	  vis_f32  tappairhi,tappairlo,splithi,splitlo;
	  int  	 outerloop;

	  vis_write_gsr((scalefactor+1)<<3);	  
	  
	  accumpair0 = vis_fzero();
	  tapptr0 = (double *) shift_taparray;
	  accumpair1 = vis_fzero();
	  tapptr1 = (double *) (shift_taparray + 4*(numloop));
	  accumpair2 = vis_fzero();
	  tapptr2 = (double *) ((shift_taparray) + 8*(numloop));
	  accumpair3 = vis_fzero();
	  tapptr3 = (double *) ((shift_taparray) + 12*(numloop));

	  /* filter data */
	  for (outerloop = 0; outerloop < (numloop); outerloop++) {
	    /* set up data */
	    datahi = vis_read_hi(double(signalIn%outerloop));
	    datalo = vis_read_lo(double(signalIn%outerloop));
	    /* calculate four outputs */
	    /*
	     * 0: set up tap pairs for each
	     * shifted taparray
	     */
	    tapvalue = *tapptr0++;
	    tappairhi = vis_read_hi(tapvalue);
	    tappairlo = vis_read_lo(tapvalue);
	    /* take inner products */
	    pairlolo = vis_fmuld8sux16(datalo, tappairlo);
	    pairlohi = vis_fmuld8ulx16(datalo, tappairlo);
	    pairhilo = vis_fmuld8sux16(datahi, tappairhi);
	    pairhihi = vis_fmuld8ulx16(datahi, tappairhi);
	    /* accumulate results */
	    pairlo = vis_fpadd32(pairlolo, pairlohi);
	    pairhi = vis_fpadd32(pairhilo, pairhihi);
	    pair = vis_fpadd32(pairhi, pairlo);
	    accumpair0 = vis_fpadd32(accumpair0,pair);
	    /*
	     * 1: set up tap pairs for each
	     * shifted taparray
	     */
	    tapvalue = *tapptr1++;
	    tappairhi = vis_read_hi(tapvalue);
	    tappairlo = vis_read_lo(tapvalue);
	    /* take inner products */
	    pairlolo = vis_fmuld8sux16(datalo, tappairlo);
	    pairlohi = vis_fmuld8ulx16(datalo, tappairlo);
	    pairhilo = vis_fmuld8sux16(datahi, tappairhi);
	    pairhihi = vis_fmuld8ulx16(datahi, tappairhi);
	    /* accumulate results */
	    pairlo = vis_fpadd32(pairlolo, pairlohi);
	    pairhi = vis_fpadd32(pairhilo, pairhihi);
	    pair = vis_fpadd32(pairhi, pairlo);
	    accumpair1 = vis_fpadd32(accumpair1,pair);
	    /*
	     * 2: set up tap pairs for each
	     * shifted taparray
	     */
	    tapvalue = *tapptr2++;
	    tappairhi = vis_read_hi(tapvalue);
	    tappairlo = vis_read_lo(tapvalue);
	    /* take inner products */
	    pairlolo = vis_fmuld8sux16(datalo, tappairlo);
	    pairlohi = vis_fmuld8ulx16(datalo, tappairlo);
	    pairhilo = vis_fmuld8sux16(datahi, tappairhi);
	    pairhihi = vis_fmuld8ulx16(datahi, tappairhi);
	    /* accumulate results */
	    pairlo = vis_fpadd32(pairlolo, pairlohi);
	    pairhi = vis_fpadd32(pairhilo, pairhihi);
	    pair = vis_fpadd32(pairhi, pairlo);
	    accumpair2 = vis_fpadd32(accumpair2,pair);
	    /*
	     * 3: set up tap pairs for each
	     * shifted taparray
	     */
	    tapvalue = *tapptr3++;
	    tappairhi = vis_read_hi(tapvalue);
	    tappairlo = vis_read_lo(tapvalue);
	    /* take inner products */
	    pairlolo = vis_fmuld8sux16(datalo, tappairlo);
	    pairlohi = vis_fmuld8ulx16(datalo, tappairlo);
	    pairhilo = vis_fmuld8sux16(datahi, tappairhi);
	    pairhihi = vis_fmuld8ulx16(datahi, tappairhi);
	    /* accumulate results */
	    pairlo = vis_fpadd32(pairlolo, pairlohi);
	    pairhi = vis_fpadd32(pairhilo, pairhihi);
	    pair = vis_fpadd32(pairhi, pairlo);
	    accumpair3 = vis_fpadd32(accumpair3,pair);
	  }
	  /*
	   * sum accumulators and pack outputs into a
	   * double
	   */
	  splithi = vis_read_hi(accumpair0);
	  splitlo = vis_read_lo(accumpair0);
	  r3 = vis_fpadd32s(splithi, splitlo);
	  splithi = vis_read_hi(accumpair1);
	  splitlo = vis_read_lo(accumpair1);
	  r2 = vis_fpadd32s(splithi, splitlo);    
	  splithi = vis_read_hi(accumpair2);
	  splitlo = vis_read_lo(accumpair2);
	  r1 = vis_fpadd32s(splithi, splitlo);
	  splithi = vis_read_hi(accumpair3);
	  splitlo = vis_read_lo(accumpair3);
	  r0 = vis_fpadd32s(splithi, splitlo);
	  dhi = vis_freg_pair(r3,r2);
	  dlo = vis_freg_pair(r1,r0);
	  packedhi = vis_fpackfix(dhi);
	  packedlo = vis_fpackfix(dlo);

	  signalOut%0 << vis_freg_pair(packedhi, packedlo);
	}
}	






