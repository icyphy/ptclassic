#include <stdlib.h>
#include <stdio.h>
#include "vis_types.h"
#include "vis_proto.h"

void vdk_vis_ptolemyfir(double* src, double* dst1, int dlen, double*
		   shift_taparray, int numloop)
{
  double	*tapptr0,*tapptr1,*tapptr2,*tapptr3, tapvalue;
  double	pairlohi, pairlolo, pairhilo,pairhihi;
  double 	pairlo, pairhi, pair, packedOut,*packedaccum;
  double 	result[2];
  double	accumpair0, accumpair1, accumpair2, accumpair3;
  float  	packedhi, packedlo, datahi,datalo;
  float 	tappairhi, tappairlo, splithi,splitlo;
  float		*r;
  int  		outerloop, nminusk;
  int	     	n,i,j,k;
  short 	*t0,*t1,*t2;

  for(i=0;i<dlen;i++){
    accumpair0 = vis_fzero();
    tapptr0 = (double *) (shift_taparray);

    accumpair1 = vis_fzero();
    tapptr1 = (double *) (shift_taparray + numloop);

    accumpair2 = vis_fzero();
    tapptr2 = (double *) (shift_taparray + 2 * numloop);

    accumpair3 = vis_fzero();
    tapptr3 = (double *) (shift_taparray + 3 * numloop);

    nminusk = i;
    n = numloop;
    if(nminusk<numloop)
      n=nminusk+1;
    /* filter data */
    for (outerloop = 0; outerloop < n; outerloop++) {
      /* set up data */
      datahi = vis_read_hi(src[nminusk]);
      datalo = vis_read_lo(src[nminusk]);
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
      
      nminusk--;
    }
    /*
     * sum accumulators and pack outputs into a
     * double
     */
    r=(float *) result;

    splithi = vis_read_hi(accumpair0);
    splitlo = vis_read_lo(accumpair0);
    r[3] = vis_fpadd32s(splithi, splitlo);
    
    splithi = vis_read_hi(accumpair1);
    splitlo = vis_read_lo(accumpair1);
    r[2] = vis_fpadd32s(splithi, splitlo);
    
    splithi = vis_read_hi(accumpair2);
    splitlo = vis_read_lo(accumpair2);
    r[1] = vis_fpadd32s(splithi, splitlo);
    
    splithi = vis_read_hi(accumpair3);
    splitlo = vis_read_lo(accumpair3);
    r[0] = vis_fpadd32s(splithi, splitlo);

    packedhi = vis_fpackfix(result[0]);
    packedlo = vis_fpackfix(result[1]);
    dst1[i] = vis_freg_pair(packedhi, packedlo);
  }
}


