#include <stdlib.h>
#include <stdio.h>
#include "vis_types.h"
#include "vis_proto.h"

void vdk_vis_ptolemyiir(vis_d64* src, vis_d64* dst, int dlen, vis_d64
			quadn0, double alltaps, int scalefactor)
{
  int 		i;
  double 	upper,lower,splitresult;
  double        allstates,accum,quadstate,quadresult;
  float         resulthi,resultlo,result;
  vis_u32	ff1,ff2,su0,su1,su2,su3;
  vis_s32	resultl0,resultl1,resultl2,resultl3;
  short		state10,state11,state12,state13;
  short 	*inarray,in0,in1,in2,in3;

  inarray = (short*) src;
  allstates = vis_fzero();
  ff1= vis_fzeros();
  for (i = 0; i < dlen; i++) {
    in0=*inarray++;
    in1=*inarray++;
    in2=*inarray++;
    in3=*inarray++;
    /* 0 */
    /* find product of states-taps */
    upper = vis_fmul8sux16(allstates,alltaps);
    lower = vis_fmul8ulx16(allstates,alltaps);
    splitresult = vis_fpadd16(upper,lower);
    resulthi = vis_read_hi(splitresult);
    resultlo = vis_read_lo(splitresult);
    result = vis_fpadd16s(resulthi,resultlo);
    resultl0 = *((vis_s32*) &result);
    /* find next_state */
    ff2=ff1;
    state10 = (in0 - (resultl0>>15)) << scalefactor;
    ff1 = (state10 << 16 | state10 & 0xffff);
    allstates=vis_to_double(ff1,ff2);
    /* 1 */
    /* find product of states-taps */
    upper = vis_fmul8sux16(allstates,alltaps);
    lower = vis_fmul8ulx16(allstates,alltaps);
    splitresult = vis_fpadd16(upper,lower);
    resulthi = vis_read_hi(splitresult);
    resultlo = vis_read_lo(splitresult);
    result = vis_fpadd16s(resulthi,resultlo);
    resultl1 = *((vis_s32*) &result);
    /* find next_state */
    ff2=ff1;
    state11 = (in1 - (resultl1>>15)) << scalefactor;
    ff1 = (state11 << 16 | state11 & 0xffff);
    allstates=vis_to_double(ff1,ff2);
    /* 2 */
    /* find product of states-taps */
    upper = vis_fmul8sux16(allstates,alltaps);
    lower = vis_fmul8ulx16(allstates,alltaps);
    splitresult = vis_fpadd16(upper,lower);
    resulthi = vis_read_hi(splitresult);
    resultlo = vis_read_lo(splitresult);
    result = vis_fpadd16s(resulthi,resultlo);
    resultl2 = *((vis_s32*) &result);
    /* find next_state */
    ff2=ff1;
    state12 = (in2 - (resultl2>>15)) << scalefactor;
    ff1 = (state12 << 16 | state12 & 0xffff);
    allstates=vis_to_double(ff1,ff2);
    /* 3 */
    /* find product of states-taps */
    upper = vis_fmul8sux16(allstates,alltaps);
    lower = vis_fmul8ulx16(allstates,alltaps);
    splitresult = vis_fpadd16(upper,lower);
    resulthi = vis_read_hi(splitresult);
    resultlo = vis_read_lo(splitresult);
    result = vis_fpadd16s(resulthi,resultlo);
    resultl3 = *((vis_s32*) &result);
    /* find next_state */
    ff2=ff1;
    state13 = (in3 - (resultl3>>15)) << scalefactor;
    ff1 = (state13 << 16 | state13 & 0xffff);
    allstates=vis_to_double(ff1,ff2);
    /* calculate output */
    su0 = (state10 << 16 | state11 & 0xffff);
    su1 = (state12 << 16 | state13 & 0xffff);
    quadstate = vis_to_double(su0,su1);
    upper = vis_fmul8sux16(quadn0,quadstate);
    lower = vis_fmul8ulx16(quadn0,quadstate);
    accum = vis_fpadd16(upper,lower);
    su2 = (resultl0 << 16 | resultl1 & 0xffff);
    su3 = (resultl2 << 16 | resultl3 & 0xffff);
    quadresult = vis_to_double(su2,su3);
    *dst++ = vis_fpadd16(accum,quadresult);
  }
}

