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
  float         resulthi,resultlo,result;
  vis_f32	ff1,ff2;
  vis_u32	fu;
  vis_s32	resultl;
  short		state1,r0,r1;

  allstates = vis_fzero();
  ff1=ff2 = vis_fzeros();
  for (i = 0; i < dlen; i++) {
    /* 0 */
    /* find product of states-taps */
    upper = vis_fmul8sux16(allstates,alltaps);
    lower = vis_fmul8ulx16(allstates,alltaps);
    splitresult = vis_fpadd16(upper,lower);
    resulthi = vis_read_hi(splitresult);
    resultlo = vis_read_lo(splitresult);
    result = vis_fpadd16s(resulthi,resultlo);
    resultl = *((vis_s32*) &result);
    /* find next_state */
    ff2=ff1;
    state1 = (src[i] - (resultl>>15)) << scalefactor;
    fu = (state1<<16 | state1 & 0xffff);
    ff1 = vis_to_float(fu);
    allstates=vis_freg_pair(ff1,ff2);
    /* find output */
    dst[i] = (n0 * state1 >> 15) + (resultl<<1);
  }  
}
