#include <stdlib.h>
#include <math.h>
#include "vis_types.h"
#include "vis_proto.h"

/***************************************************************/
#define SWAP(a, b) tempr=(a); (a)=(b); (b)=tempr

/***************************************************************/
void vdk_cfloat_ptolemyfft(double *redata, double *imdata, int nn,
			   double *cosfarray, double *sinfarray)
{
  int N2,N1,k,i,j,l,v;
  double C,S,A,E;
  double REDATATMP,IMDATATMP;

  N2=nn;
  for(k=0;k<8;k++){
    N1=N2;
    N2=N2/2;
    for(j=0;j<N2;j++){
      C=*cosfarray++;
      S=*sinfarray++;
      v=j;
      while(v<nn){
	l=v+N2;
	REDATATMP=redata[v]-redata[l];
	redata[v]=redata[v]+redata[l];
	IMDATATMP=imdata[v]-imdata[l];
	imdata[v]=imdata[v]+imdata[l];
	redata[l]=REDATATMP*C-IMDATATMP*S;
	imdata[l]=REDATATMP*S+IMDATATMP*C;
	v++;
	v+=N1-1;
      }
    }
  }
}
/***************************************************************/
void reorderfloatfft(double *s1, double *s2, int nn)
{
  unsigned long   n, j, i, m;
  double           tempr;
  
  n = nn << 1;
  j = 1;
  for (i = 1; i < n; i += 2) {
    if (j > i) {
      SWAP(s1[(j - 1) / 2], s1[(i - 1) / 2]);
      SWAP(s2[(j - 1) / 2], s2[(i - 1) / 2]);
    }
    m = n >> 1;
    while (m >= 2 && j > m) {
      j -= m;
      m >>= 1;
    }
    j += m;
  }
}
/***************************************************************/
