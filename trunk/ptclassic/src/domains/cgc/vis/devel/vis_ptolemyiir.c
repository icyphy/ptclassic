#include <stdlib.h>
#include <stdio.h>
#include "vis_types.h"
#include "vis_proto.h"

void vdk_vis_ptolemyiir(vis_s16* src, vis_s16* dst, int dlen, vis_s16
			n0, double alltaps, int scalefactor)
{
  int 		i;
  double 	upper,lower,splitresult;
  double        allstates;
  float         resulthi,resultlo,result[1];
  vis_f32	ff1,ff2;
  vis_u32	fu;
  short		state1,*statetapprod,*outarray,*invalue;

  allstates = vis_fzero();
  ff1=ff2 = vis_fzeros();
  invalue = (short *) src;
  outarray = (short *) dst;
  statetapprod = (short *) result;
  for (i = 0; i < dlen; i++) {
    /* 0 */
    /* find product of states-taps */
    upper = vis_fmul8sux16(allstates,alltaps);
    lower = vis_fmul8ulx16(allstates,alltaps);
    splitresult = vis_fpadd16(upper,lower);
    resulthi = vis_read_hi(splitresult);
    resultlo = vis_read_lo(splitresult);
    result[0] = vis_fpadd16s(resulthi,resultlo);
    /* find next_state */
    ff2=ff1;
    state1 = (src[i] - (statetapprod[0] << 1)) << scalefactor;
    fu = (state1<<16 | state1 & 0xffff);
    ff1 = vis_to_float(fu);
    allstates=vis_freg_pair(ff1,ff2);
    /* find output */
    dst[i] = (n0 * state1 >> 15) + (statetapprod[1] << 1);
    /* 1 */
    /* find product of states/taps */
    /*   upper = vis_fmul8sux16(allstates,alltaps);
    lower = vis_fmul8ulx16(allstates,alltaps);
    splitresult = vis_fpadd16(upper,lower);
    resulthi = vis_read_hi(splitresult);
    resultlo = vis_read_lo(splitresult);
    result[0] = vis_fpadd16s(resulthi,resultlo);*/
    /* find next_state */
    /*    ff2=ff1;
    state1 = (invalue[4*i+1] - (statetapprod[0] << 1)) << scalefactor;
    fu = (state1<<16 | state1 & 0xffff);
    ff1 = vis_to_float(fu);
    allstates=vis_freg_pair(ff1,ff2);*/
    /* find output */
    /* outarray[4*i+1] = (n0 * state1 >> 15) + (statetapprod[1] << 1);*/
    /* 2 */
    /* find product of states/taps */
    /* upper = vis_fmul8sux16(allstates,alltaps);
    lower = vis_fmul8ulx16(allstates,alltaps);
    splitresult = vis_fpadd16(upper,lower);
    resulthi = vis_read_hi(splitresult);
    resultlo = vis_read_lo(splitresult);
    result[0] = vis_fpadd16s(resulthi,resultlo);*/
    /* find next_state */
    /*ff2=ff1;
    state1 = (invalue[4*i+2] - (statetapprod[0] << 1)) << scalefactor;
    fu = (state1<<16 | state1 & 0xffff);
    ff1 = vis_to_float(fu);
    allstates=vis_freg_pair(ff1,ff2);*/
    /* find output */
    /*outarray[4*i+2] = (n0 * state1 >> 15) + (statetapprod[1] << 1);*/
    /* 3 */
    /* find product of states/taps */
    /*upper = vis_fmul8sux16(allstates,alltaps);
    lower = vis_fmul8ulx16(allstates,alltaps);
    splitresult = vis_fpadd16(upper,lower);
    resulthi = vis_read_hi(splitresult);
    resultlo = vis_read_lo(splitresult);
    result[0] = vis_fpadd16s(resulthi,resultlo);*/
    /* find next_state */
    /*ff2=ff1;
    state1 = (invalue[4*i+3] - (statetapprod[0] << 1)) << scalefactor;
    fu = (state1<<16 | state1 & 0xffff);
    ff1 = vis_to_float(fu);
    allstates=vis_freg_pair(ff1,ff2);*/
    /* find output */
    /*outarray[4*i+3] = (n0 * state1 >> 15) + (statetapprod[1] << 1);*/
  }  
}	

