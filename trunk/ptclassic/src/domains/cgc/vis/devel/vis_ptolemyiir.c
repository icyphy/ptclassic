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
/***************************************************************/
double mult4x4(double mult1, double mult2)
{
  double          upper,lower,prod;

  upper = vis_fmul8sux16(mult1,mult2);
  lower = vis_fmul8ulx16(mult1,mult2);
  return prod = vis_fpadd16(upper,lower);
}
/***************************************************************/
void vdk_visretimeby2_ptolemyiir(float *src, float *dst3, int length,
			      double alphatop, double alphabottom,
			      double beta, int scalefactor)
{
  double quadsrc,repeatstate,accumpair0,accumpair1,accumpair2;
  float t0,t1,t2,accum0,accum1;
  vis_s32 result0,result1;
  vis_f32 currentstate;
  vis_u32 fu;
  int i;
  short out0,out1;

  currentstate = vis_fzeros();
  for(i=0;i<length;i++){
    quadsrc = vis_freg_pair(*src++,*src);
    repeatstate = vis_freg_pair(currentstate,currentstate);
    accumpair0 = mult4x4(beta,repeatstate);
    accumpair1 = mult4x4(alphatop,quadsrc);
    accumpair2 = mult4x4(alphabottom,quadsrc);
    
    t0 = vis_read_hi(accumpair0);
    t1 = vis_read_hi(accumpair1);
    accum0 = vis_fpadd16s(t0,t1);
    t2 = vis_read_lo(accumpair1);
    accum0 = vis_fpadd16s(accum0,t2);
    result0 = *((vis_s32*) &accum0);
    out0 = (result0+(result0>>16))<<(scalefactor+1);

    t0 = vis_read_lo(accumpair0);
    t1 = vis_read_hi(accumpair2);
    accum1 = vis_fpadd16s(t0,t1);
    t2 = vis_read_lo(accumpair2);
    accum1 = vis_fpadd16s(accum1,t2);
    result1 = *((vis_s32*) &accum1);
    out1 = (result1+(result1>>16))<<(scalefactor+1);

    fu = out0 << 16 | out1 & 0xffff;
    dst3[i] = currentstate = vis_to_float(fu);
  }
}
/***************************************************************/


