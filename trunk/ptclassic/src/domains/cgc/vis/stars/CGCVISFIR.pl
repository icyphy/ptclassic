defstar {
	name { VISFIR }
	domain { CGC }
	version { $Id$ }
	author { William Chen, John Reekie, and Edward Lee}
	copyright {
Copyright (c) 1996-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC Visual Instruction Set library }
	desc { 
A finite impulse response (FIR) filter.  In order to take advantage
of the 16-bit partitioned multiplies, the VIS FIR reformulates 
the filtering operation to that of a matrix operation (Ax=y), 
where A is a tap matrix, x is an input vector, and y is an output
vector.  The matrix A is first constructed from the filter taps.  
Each row is filled by copying the filter taps, zero-padding so that 
its length is a multiple of 4, and shifting to the right by one.  
Four of these rows are used to build up matrix A.  The matrix A is 
then multiplied with the 16-bit partitioned input vector.  This is 
equivalent to taking four sum of products.  The final result is 
accumulated in four 16-bit fixed point numbers which are concatenated 
into a single 64-bit float particle.
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
	    maxpast = (int)((taplength-1)/NUMPACK+1);
	  else
	    maxpast = (int)((taplength-1)/NUMPACK+2);

	  //signalIn.setSDFParams(NUMIN,maxpast-1);
	}
	codeblock(mainDecl) {
	  /* allocate memory for tap matrix */
	  double $starSymbol(src)[$val(maxpast)];
	  int $starSymbol(currentValue),$starSymbol(nminusk);
	  vis_s16* $starSymbol(tapmatrix) =
	    (vis_s16*) memalign(sizeof(double),sizeof(vis_s16)*16*$val(maxpast));
	  int $starSymbol(taprowindex),$starSymbol(tapcolindex);
          int $starSymbol(numloop),$starSymbol(taprowlength),$starSymbol(i);
	  short *$starSymbol(indexcount),$starSymbol(scaledown);
	}
	codeblock(initialize) {
	  $starSymbol(scaledown)=(short) 1<<$val(scalefactor);

	  /* determine the length of each row of the tap matrix */
	  $starSymbol(taprowlength) = 4*$val(maxpast);

	  /* initialize tap matrix to zero*/
	  $starSymbol(indexcount) = $starSymbol(tapmatrix);
	  for($starSymbol(taprowindex)=0;$starSymbol(taprowindex)<4;$starSymbol(taprowindex)++){
	    for($starSymbol(tapcolindex)=0;$starSymbol(tapcolindex)<$starSymbol(taprowlength);
		$starSymbol(tapcolindex)++){
	      *$starSymbol(indexcount)++ = 0;
	    }

	  }
	  /* fill tap matrix with tap coefficients*/
	  for($starSymbol(taprowindex)=0;$starSymbol(taprowindex)<4;$starSymbol(taprowindex)++){
	    $starSymbol(indexcount) = $starSymbol(tapmatrix) +
	      ($starSymbol(taprowlength)+1)*($starSymbol(taprowindex));
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

	  for($starSymbol(i)=0;$starSymbol(i)<$val(maxpast);$starSymbol(i)++){
	    $starSymbol(src)[$starSymbol(i)] = 0;
	  }
	  $starSymbol(currentValue) = $starSymbol(nminusk) = 0;
	}
	initCode {
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
	  vis_write_gsr(($val(scalefactor)+1)<<3);	  

	  accumpair0 = vis_fzero();
	  tapptr0 = (double *) ($starSymbol(tapmatrix));
	  accumpair1 = vis_fzero();
	  tapptr1 = (double *) ($starSymbol(tapmatrix) + 4 * $starSymbol(numloop));
	  accumpair2 = vis_fzero();
	  tapptr2 = (double *) ($starSymbol(tapmatrix) + 8 * $starSymbol(numloop));
	  accumpair3 = vis_fzero();
	  tapptr3 = (double *) ($starSymbol(tapmatrix) + 12 * $starSymbol(numloop));

	  if($starSymbol(currentValue)>$val(maxpast)-1)
	    $starSymbol(currentValue) -= $val(maxpast);

	  $starSymbol(src)[$starSymbol(currentValue)] = $ref(signalIn);
	  $starSymbol(nminusk)= $starSymbol(currentValue);
	  /* filter data */
	  for (outerloop = 0; outerloop < $starSymbol(numloop); outerloop++) {
	    /* set up data */
	    if($starSymbol(nminusk) <0)
	      $starSymbol(nminusk) += $val(maxpast);

	    datahi = vis_read_hi((double) $starSymbol(src)[$starSymbol(nminusk)]);
	    datalo = vis_read_lo((double) $starSymbol(src)[$starSymbol(nminusk)]);
	    /* calculate four outputs */
	    /*
	     * 0: multiply first row of tap matrix with the input vector
	     * 
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
	     * 1: multiply second row of tap matrix with the input vector
	     * 
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
	     * 2: multiply third row of tap matrix with the input vector
	     * 
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
	     * 3: multiply fourth row of tap matrix with the input vector
	     * 
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

	    $starSymbol(nminusk)--;
	  }
	  /*
	   * sum accumulators and pack outputs into a
	   * 64-bit float particle
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
	  $starSymbol(currentValue)++;
	}
	go {   
	  addCode(localDecl);
	  addCode(filter);
	}
	wrapup {
	  addCode("free($starSymbol(tapmatrix));");
	}
}

