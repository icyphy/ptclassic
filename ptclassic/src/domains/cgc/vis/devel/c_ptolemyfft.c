/* User:      williamc
   Date:      Fri May 31 05:35:27 1996
   Target:    CGCVIS
   Universe:  fftcomp */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <math.h>
#define NUMTIMES (10)
#if !defined(NO_FIX_SUPPORT)
/* include definitions for the fix runtime library */
#include "CGCrtlib.h"
#endif

#if !defined(COMPLEX_DATA)
#define COMPLEX_DATA 1
 typedef struct complex_data { double real; double imag; } complex; 
#endif

extern int main(int argc, char *argv[]);


/*
 * This fft routine is from ~gabriel/src/filters/fft/fft.c;
 * I am unsure of the original source.  The file contains no
 * copyright notice or description.
 * The declaration is changed to the prototype form but the
 * function body is unchanged.  (J. T. Buck)
 */

#define SWAP(a, b) tempr=(a); (a)=(b); (b)=tempr

/*
 * Replace data by its discrete Fourier transform, if isign is
 * input as 1, or by its inverse discrete Fourier transform, if 
 * "isign" is input as -1.  "data'"is a complex array of length "nn",
 * input as a real array data[0..2*nn-1]. "nn" MUST be an integer
 * power of 2 (this is not checked for!?)
 */
static void fft_rad2(double *redata,double *imdata, int nn)
{
  int N2,N1,k,i,j,l,v;
  double C,S,A,E;
  double REDATATMP,IMDATATMP;

  N2=nn;
  for(k=0;k<8;k++){
    N1=N2;
    N2=N2/2;
    E=6.28318/N1;
    A=0;
    for(j=0;j<N2;j++){
      C=cos(A);
      S=-sin(A);
      A=j*E;
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
static void reorderfft(double *s1, double *s2, int nn)
{
  unsigned long   n, j, i, m;
  double           tempr;

  n = nn << 1;
  j = 1;
  for (i = 1; i < n; i += 2) {
    if (j > i) {
      SWAP(s1[(j - 1) / 2], s1[(i - 1) / 2]);
      SWAP(s2[(j - 1) / 2 - 1], s2[(i - 1) / 2]);
    }
    m = n >> 1;
    while (m >= 2 && j > m) {
      j -= m;
      m >>= 1;
    }
    j += m;
  }
}
static void reorder(data, nn, isign)
double* data;
int nn, isign;
{
	int	n;
	int	mmax;
	int	m, j, istep, i;
	double	wtemp, wr, wpr, wpi, wi, theta;
	double	tempr, tempi;

	data--;
	n = nn << 1;
	j = 1;

	for (i = 1; i < n; i += 2) {
		if(j > i) {
			SWAP(data[j], data[i]);
			SWAP(data[j+1], data[i+1]);
		}
		m= n >> 1;
		while (m >= 2 && j >m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax = 2;
	while (n > mmax) {
		istep = 2*mmax;
		theta = -6.28318530717959/(isign*mmax);
		wtemp = sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m = 1; m < mmax; m += 2) {
			for (i = m; i < n; i += istep) {
				j = i + mmax;
				tempr = wr*data[j] - wi*data[j+1];
				tempi = wr*data[j+1] + wi*data[j];
				data[j] = data[i] - tempr;
				data[j+1] = data[i+1] - tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr = (wtemp=wr)*wpr - wi*wpi+wr;
			wi = wi*wpr + wtemp*wpi + wi;
		}
		mmax = istep;
	}
}

/* main function */
int main(int argc, char *argv[]) {
  FILE* fp_11[1];
  FILE* fp_13[1];
double value_16;
double output_0;
complex output_1[256];
int output_9;
double output_4;
double output_5;
double localData_17[256];
double localData_18[256];
complex output_6[256];
int input_10;
double real_7;
double imag_8;
int count_21;
double index_22;
int count_19;
double index_20;
hrtime_t start, end;
float    time1, time2, time3, meansampletime, runningtotal;

count_19=0;
index_20=0.0;
count_21=0;
index_22=0.0;
{int i; for(i=0;i<512;i++) localData_17[i]=0.0;}
value_16=0.0;
output_0 = 0.0;
{int i; for(i=0;i<256;i++) output_1[i].real = output_1[i].imag = 0.0;}
output_9 = 0;
output_4 = 0.0;
output_5 = 0.0;
{int i; for(i=0;i<256;i++) output_6[i].real = output_6[i].imag = 0.0;}
input_10 = 0;
real_7 = 0.0;
imag_8 = 0.0;
 
runningtotal=0.0;

    if(!(fp_11[0] = fopen("CGCVIS_temp_120","w")))
    {
        fprintf(stderr,"ERROR: cannot open output file for Xgraph star.\n");
	exit(1);
    }
    if(!(fp_13[0] = fopen("CGCVIS_temp_140","w")))
    {
        fprintf(stderr,"ERROR: cannot open output file for Xgraph star.\n");
	exit(1);
    }
{ int sdfLoopCounter_23;for (sdfLoopCounter_23=0; sdfLoopCounter_23 < NUMTIMES; sdfLoopCounter_23++) {
    { int sdfLoopCounter_15;for (sdfLoopCounter_15=0; sdfLoopCounter_15 < 256; sdfLoopCounter_15++) {
	{  /* star fftcomp.expgen1.Ramp1 (class CGCRamp) */
	output_0 = value_16;
	value_16 += 0.0628318530717958;
	}
	{  /* star fftcomp.expgen1.expjx1.Fork.output=21 (class CGCFork) */
	}
	{  /* star fftcomp.expgen1.expjx1.Cos1 (class CGCCos) */
	output_4 = cos(output_0);
	}
	{  /* star fftcomp.expgen1.expjx1.Sin1 (class CGCSin) */
	output_5 = sin(output_0);
	}
	{  /* star fftcomp.expgen1.expjx1.RectToCx1 (class CGCRectToCx) */
	output_1[output_9].real = output_4;
	output_1[output_9].imag = output_5;
	output_9 += 1;
	if (output_9 >= 256)
		output_9 -= 256;
	}
}} /* end repeat, depth 1*/
	{  /* star fftcomp.FFTCx1 (class CGCFFTCx) */
	int i, j = 0;
	int k=0;
	for (i = 256 - 1; i >= 0; i--) {
		localData_17[j++] = output_1[(255-(i))].real;
		localData_18[k++] = output_1[(255-(i))].imag;
	}
        start = gethrtime();
	fft_rad2 (localData_17,localData_18,256);
        end = gethrtime();
        time3 = (float) (end - start);
        runningtotal+=time3;

	reorderfft(localData_17,localData_18,256);
	j = 0;
	k=0;
	for (i = 256 - 1; i >= 0; i--) {
		output_6[(255-(i))].real = localData_17[j++];
		output_6[(255-(i))].imag = localData_18[k++];
	}
	}
    { int sdfLoopCounter_18;for (sdfLoopCounter_18=0; sdfLoopCounter_18 < 256; sdfLoopCounter_18++) {
	{  /* star fftcomp.CxToRect1 (class CGCCxToRect) */
	real_7 = output_6[input_10].real;
	imag_8 = output_6[input_10].imag;
	input_10 += 1;
	if (input_10 >= 256)
		input_10 -= 256;
	}
	{  /* star fftcomp.XMgraph2 (class CGCXMgraph) */
	if (++count_19 >= 0) {
		fprintf(fp_13[0],"%g %g\n",index_20,imag_8);
	}
	index_20 += 1.0;
	}
	{  /* star fftcomp.XMgraph1 (class CGCXMgraph) */
	if (++count_21 >= 0) {
		fprintf(fp_11[0],"%g %g\n",index_22,real_7);
		/*printf("%g %g\n",index_22,real_7);*/

	}
	index_22 += 1.0;
	}
}} /* end repeat, depth 1*/
}} /* end repeat, depth 0*/
{ int i;
for (i = 0; i < 1; i++) fclose(fp_11[i]);
system("( pxgraph -t 'real' -bb -tk =800x400 CGCVIS_temp_120 ;/bin/rm -f CGCVIS_temp_120) &");
}
{ int i;
for (i = 0; i < 1; i++) fclose(fp_13[i]);
system("( pxgraph -t 'imag' -bb -tk =800x400 CGCVIS_temp_140 ; /bin/rm -f CGCVIS_temp_140) &");
    }
meansampletime=(float)runningtotal/(NUMTIMES)/1000;
printf("mean sample time =%f microsec\n",meansampletime);

return 1;
}
