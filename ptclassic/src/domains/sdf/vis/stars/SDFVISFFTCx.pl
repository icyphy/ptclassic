defstar {
	name { VISFFTCx }
	domain { SDF }
	version { $Id$ }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF vis library }
	desc { 
A single complex sequence FFT using radix 2.
Input length must be power of two.
	}
	input {
	  name { realIn }
	  type { float }
	  desc { Input float type }
	}	
	input {	
	  name { imagIn }
	  type { float }
	  desc { Output float type }
	}
	output {	
	  name { realOut }
	  type { float }
	  desc { Output float type }
	}
	output {	
	  name { imagOut }
	  type { float }
	  desc { Output float type }
	}
      	defstate {
	  name { sizeoffft }
	  type { int }
	  default { 32 }
	  desc { FFT Size }
	  attributes { A_CONSTANT|A_SETTABLE }
	}
      	defstate {
	  name { orderfft }
	  type { int }
	  default { 5 }
	  desc { Log2 of fft size }
	  attributes { A_CONSTANT|A_SETTABLE }
	}
      	defstate {
	  name { scale }
	  type { float }
	  default { "32767.0" }
	  desc { Filter tap scale }
	  attributes { A_CONSTANT|A_SETTABLE }
	}
        ccinclude {<vis_proto.h>, <math.h>, <stdio.h>}
	code {
#define NUMPACK (4)
#define SWAP(a, b) tempr=(a); (a)=(b); (b)=tempr
	  static void calcTwSinCos(double *Twsine,double
				   *Twcosine,double ExpofW,int N2)
	    {
	      double scale = 32767.0;
	      int i;
	      short *indexcount0,*indexcount1;

	      indexcount0 = (short *) Twcosine;
	      indexcount1 = (short *) Twsine;
	      for(i=0;i<N2;i++){
		*indexcount0++ = (short) scale*cos(ExpofW*(N2-1-i));
		*indexcount1++ = (short) -scale*sin(ExpofW*(N2-1-i));       
	      }
	    }
	  static double mult4x4(double mult1,double mult2)
	    { 

	      double prodhihi,prodhilo,prodlohi,prodlolo;
	      double prodhi,prodlo,product;
	      float mult1hi,mult1lo,mult2hi,mult2lo;
	      float packhi,packlo;

	      mult1hi = vis_read_hi(mult1);
	      mult1lo = vis_read_lo(mult1);
	      mult2hi = vis_read_hi(mult2);
	      mult2lo = vis_read_lo(mult2);
	      
	      prodhihi = vis_fmuld8sux16(mult1hi,mult2hi);
	      prodhilo = vis_fmuld8ulx16(mult1hi,mult2hi);
	      prodlohi = vis_fmuld8sux16(mult1lo,mult2lo);
	      prodlolo = vis_fmuld8ulx16(mult1lo,mult2lo);

	      prodhi = vis_fpadd32(prodhihi,prodhilo);
	      prodlo = vis_fpadd32(prodlohi,prodlolo);
	      packhi = vis_fpackfix(prodhi);
	      packlo = vis_fpackfix(prodlo);
	      return product = vis_freg_pair(packhi,packlo);
	    }
	  static float mult2x2(float mult1,float mult2)
	    {
	      double resultu,resultl,result;
	      float product;

	      resultu = vis_fmuld8sux16(mult1,mult2);
	      resultl = vis_fmuld8ulx16(mult1,mult2);
	      result = vis_fpadd32(resultu,resultl);
	      return product = vis_fpackfix(result);
	    }
	  static void reorderfft(short *s1,short *s2, int nn)
	    {
	      unsigned long n,j,i,m;
	      short tempr;

	      n=nn<<1;
	      j=1;
	      for(i=1;i<n;i+=2){
		if(j>i){
		  SWAP(s1[nn-1-(j-1)/2],s1[nn-1-(i-1)/2]);
		  SWAP(s2[nn-(j-1)/2-1],s2[nn-(i-1)/2-1]);
		}
		m=n>>1;
		while(m>=2 && j>m){
		  j-=m;
		  m>>=1;
		}
		j+=m;
	      }
	    }
	}
	protected {
	  double *rein,*imin,*Twcosine,*Twsine;
	  double twopi;
	}
	constructor {
	  rein = imin = Twcosine = Twsine =  0;
	  twopi = 6.28318530717959;
	}
	destructor {
	  free(rein);
	  free(imin);
	  free(Twcosine);
	  free(Twsine);
	}
	begin {
	  int i;
	  short *indexcount;

	  //allocate memory
	  free(rein);
	  free(imin);
	  free(Twcosine);
	  free(Twsine);
	  rein = (double *)
		memalign(sizeof(double),sizeof(double)*sizeoffft/4);
	  imin = (double *)
	  	memalign(sizeof(double),sizeof(double)*sizeoffft/4);
	  Twcosine = (double *)
	  	memalign(sizeof(double),sizeof(double)*sizeoffft/8);
	  Twsine = (double *)
	  	memalign(sizeof(double),sizeof(double)*sizeoffft/8);
	}
	setup {
          realIn.setSDFParams(sizeoffft/4,sizeoffft/4-1);
          imagIn.setSDFParams(sizeoffft/4,sizeoffft/4-1);
          realOut.setSDFParams(sizeoffft/4,sizeoffft/4-1);
	  imagOut.setSDFParams(sizeoffft/4,sizeoffft/4-1);
	}
	go {	
	  double ExpofW,reindtmp,imagindtmp;
	  double xtcd,ytsd,xtsd,ytcd;
	  float xtcf,ytsf,xtsf,ytcf;
	  float *splitf_rein,*splitf_imin;
	  float reinftmp,imaginftmp;
	  float *Cf,*Sf;
	  int i,j,k,l;
	  int N,N1,N2;
	  short *splits_rein,*splits_imin;
	  short reinstmp,imaginstmp;

	  vis_write_gsr(8);

	  splitf_rein = (float*)rein;
	  splitf_imin = (float*)imin;
	  splits_rein = (short*)rein;
	  splits_imin = (short*)imin;

	  // read in the input
	  for(i=0;i<sizeoffft/4;i++){
		rein[i] = double(realIn%i);
		imin[i] = double(imagIn%i);
	  }

	  //first stages of fft (order of fft minus last two)
	  N=sizeoffft;
	  N2=N;
	  for(k=0;k<orderfft-2;k++){
	    N1=N2;
	    N2=N2/2;
	    ExpofW=twopi/N1;
	    for(j=0;j<N2/4;j++){
	      calcTwSinCos(Twsine,Twcosine,ExpofW,N2);
	      for(i=j;i<N/4;i+=N1/4){
		l=i+N2/4;
		reindtmp=vis_fpsub16(rein[l],rein[i]);
		rein[l]=vis_fpadd16(rein[i],rein[l]);
		imagindtmp=vis_fpsub16(imin[l],imin[i]);
		imin[l]=vis_fpadd16(imin[i],imin[l]);
		xtcd=mult4x4(reindtmp,Twcosine[j]);
		xtsd=mult4x4(reindtmp,Twsine[j]);
		ytsd=mult4x4(imagindtmp,Twsine[j]);
		ytcd=mult4x4(imagindtmp,Twcosine[j]);
		rein[i]=vis_fpsub16(xtcd,ytsd);
		imin[i]=vis_fpadd16(xtsd,ytcd);
	      }
	    }
	  }
	  //second to last stage of the fft
	      N1=N2;
	  N2=N2/2;
	  ExpofW=twopi/N1;
	  calcTwSinCos(Twsine,Twcosine,ExpofW,N2);
	  Cf=(float *) Twcosine;
	  Sf=(float *) Twsine;
	  j=0;
	  for(i=0;i<N/4;i++){
	    reinftmp=vis_fpsub16s(splitf_rein[j+1],splitf_rein[j]);
	    imaginftmp=vis_fpsub16s(splitf_imin[j+1],splitf_imin[j]);
	    splitf_rein[j+1]=vis_fpadd16s(splitf_rein[j+1],splitf_rein[j]);
	    splitf_imin[j+1]=vis_fpadd16s(splitf_imin[j+1],splitf_imin[j]);
	    xtcf=mult2x2(reinftmp,*Cf);
	    xtsf=mult2x2(reinftmp,*Sf);
	    ytsf=mult2x2(imaginftmp,*Sf);
	    ytcf=mult2x2(imaginftmp,*Cf);
	    splitf_rein[j]=vis_fpsub16s(xtcf,ytsf);
	    splitf_imin[j]=vis_fpadd16s(xtsf,ytcf);
	    j+=2;
	  }
	  //last stage of the fft
	  i=0;
	  for(j=0;j<N/2;j++){
	    reinstmp=splits_rein[i+1]-splits_rein[i];
	    imaginstmp=splits_imin[i+1]-splits_imin[i];
	    splits_rein[i+1]=splits_rein[i]+splits_rein[i+1];
	    splits_imin[i+1]=splits_imin[i]+splits_imin[i+1];
	    splits_rein[i]=reinstmp;
	    splits_imin[i]=imaginstmp;
	    i+=2;
	  }	
	  reorderfft(splits_rein,splits_imin,sizeoffft);
	  //output the results
	  for(i=0;i<sizeoffft/4;i++){
	    realOut%i << rein[i];
	    imagOut%i << imin[i];
	  }
	}	
}
