#include <stdlib.h>
#include "vis_types.h"
#include "vis_proto.h"

/***************************************************************/
void vdk_c_ptolemyfir(vis_s16 *src, vis_s16 *dst, int dlen, 
                   vis_s16 *fir, int flen)
{
    int i,k,nminusk,n;
    vis_s32 accum;

    for(i=0;i<dlen;i++){
      accum = 0;
      nminusk=i;
      n = flen;
      if (nminusk < flen) {
	n = nminusk+1;
      }
      for (k = 0; k < n; k++ ) {
	accum += fir[k] * src[nminusk];
	nminusk--;
      }
      dst[i] = (accum>>15);
    }
}

/***************************************************************/
