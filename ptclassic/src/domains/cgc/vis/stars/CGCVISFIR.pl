defstar {
	name { VISFIR }
	domain { CGC }
	derivedFrom { VISBase }
	version { $Id$ }
	author { William Chen, John Reekie, and Edward Lee}
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC Visual Instruction Set library }
	desc { A finite impulse response (FIR) filter. }
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
        defstate {
                name {taplength}
                type {int}
                default {0}
                attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
        defstate {
                name {tappadlength}
                type {int}
                default {0}
                attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
        defstate {
                name {maxpast}
                type {int}
                default {0}
                attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	code {
#define NUMIN (1)
#define	NUMPACK (4)
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
	codeblock(mainDecl) {
	  short* $starSymbol(shift_taparray) =
	    (short*)memalign(sizeof(double),sizeof(short)*4*$val(tappadlength));
	  int $starSymbol(taprowindex), $starSymbol(tapcolindex),$starSymbol(numloop);
	  short *$starSymbol(indexcount),$starSymbol(scaledown);
	}
	codeblock(initialize) {
	  vis_write_gsr(($val(scalefactor)+1)<<3);	  
	  $starSymbol(scaledown)=(short) 1<<$val(scalefactor);

	  /* initialize shifted taparrays to zero*/
	  $starSymbol(indexcount) = $starSymbol(shift_taparray);
	  for($starSymbol(taprowindex)=0;$starSymbol(taprowindex)<4;$starSymbol(taprowindex)++){
	    for($starSymbol(tapcolindex)=0;$starSymbol(tapcolindex)<$val(tappadlength);
		$starSymbol(tapcolindex)++){
	      *$starSymbol(indexcount)++ = 0;
	    }
	  }
	  /* fill taparrays*/
	  for($starSymbol(taprowindex)=0;$starSymbol(taprowindex)<4;$starSymbol(taprowindex)++){
	    $starSymbol(indexcount) = $starSymbol(shift_taparray) +
	      ($val(tappadlength)+1)*($starSymbol(taprowindex));
	    for($starSymbol(tapcolindex)=0;$starSymbol(tapcolindex)<$val(taplength);
		$starSymbol(tapcolindex)++){
	      /* scale and cast taps to short */
	      *$starSymbol(indexcount)++ = (short) $val(scale)/$starSymbol(scaledown)*
		$ref2(taps,$starSymbol(tapcolindex));
	    }
	  }	  	
	  /* loop once for each set of filter taps*/
	  if ($val(taplength) == 0)
	    $starSymbol(numloop) = 0;	
	  else		
	    $starSymbol(numloop) = $val(maxpast);	
	}
	initCode {
	  CGCVISBase::initCode();
          addInclude("<vis_proto.h>");
          addInclude("<vis_types.h>");
	  addDeclaration(mainDecl);
	  addCode(initialize);
	}
	codeblock(localDecl) {
	  vis_d64 *tapptr0,*tapptr1,*tapptr2,*tapptr3,tapvalue;
	  vis_d64 pairlohi,pairlolo,pairhilo,pairhihi,dhi,dlo;
	  vis_d64 pairlo,pairhi,pair,*packedaccum;
	  vis_d64 accumpair0,accumpair1,accumpair2,accumpair3;
	  vis_f32  packedhi,packedlo,datahi,datalo,r0,r1,r2,r3;
	  vis_f32  tappairhi,tappairlo,splithi,splitlo;
	  int  	 outerloop;
	}
	codeblock(filter) {	  
	  accumpair0 = vis_fzero();
	  tapptr0 = (double *) ($starSymbol(shift_taparray));
	  accumpair1 = vis_fzero();
	  tapptr1 = (double *) ($starSymbol(shift_taparray) + 4*$starSymbol(numloop));
	  accumpair2 = vis_fzero();
	  tapptr2 = (double *) ($starSymbol(shift_taparray) + 8 * $starSymbol(numloop));
	  accumpair3 = vis_fzero();
	  tapptr3 = (double *) ($starSymbol(shift_taparray) + 12 * $starSymbol(numloop));

	  /* filter data */
	  for (outerloop = 0; outerloop < $starSymbol(numloop); outerloop++) {
	    /* set up data */
	    datahi = vis_read_hi((double) $ref2(signalIn,outerloop));
	    datalo = vis_read_lo((double) $ref2(signalIn,outerloop));
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

	  $ref(signalOut) = vis_freg_pair(packedhi, packedlo);
	}
	go {   
	  addCode(localDecl);
	  addCode(filter);
	}
	wrapup {
	  addCode("free($starSymbol(shift_taparray));");
	}
}

