#include <stdlib.h>
#include <stdio.h>
#include "vis_types.h"
#include "vis_proto.h"

/***************************************************************/
void vdk_cint_ptolemyiir(vis_s16 *src, vis_s16 *dst, int dlen, vis_s16
			 *taps0, int scalefactor)
{
    int i;
    vis_s32 nextstate,state1,state2;

    nextstate=state1=state2=0;
    for(i=0;i<dlen;i++){
      nextstate = (src[i] - ((taps0[0]*state1+taps0[1]*state2)>>15))<<scalefactor;
      dst[i] = (taps0[2]*nextstate + taps0[3]*state1 + taps0[4]*state2)>>15;
      state2=state1;
      state1=nextstate;
     }
 }

/***************************************************************/
void vdk_cfloat_ptolemyiir(vis_d64 *src, vis_d64 *dst, int dlen, vis_d64 *taps1)
{
    int i;
    vis_d64 nextstate,state1,state2;

    nextstate=state1=state2=0.0;
    for(i=0;i<dlen;i++){   
      nextstate = src[i] - taps1[0] * state1 - taps1[1] * state2;
      dst[i] = taps1[2]*nextstate + taps1[3]*state1 + taps1[4]*state2;
      state2=state1;
      state1=nextstate;
    }
 }

/***************************************************************/
