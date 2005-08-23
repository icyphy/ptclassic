defstar {
	name { VISFFTCx }
	domain { SDF }
	version { @(#)SDFVISFFTCx.pl	1.6	7/10/96 }
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
	hinclude {<vis_types.h>}
        ccinclude {<vis_proto.h>, <math.h>, <stdio.h>}
	code {
#define NUMPACK (4)
#define M_TWO_PI (2*M_PI)
#define SWAP(a, b) tempr=(a); (a)=(b); (b)=tempr
	  static void calcTwSinCos(vis_s16 *sinsarray,vis_s16
				   *cossarray,int order, int size)
	    {
	      vis_d64 twopioverN2;
	      vis_s16 *indexcounts0,*indexcounts1;
	      int N2,i,j;

	      N2=size;
	      indexcounts0=cossarray;
	      indexcounts1=sinsarray;
	      for(i=0;i<order-2;i++){
		twopioverN2 = M_TWO_PI/N2;
		N2 = N2/2;
		for(j=0;j<N2;j++){
		  *indexcounts0++=(vis_s16) 32767* cos(twopioverN2*j);
		  *indexcounts1++=(vis_s16) -32767*sin(twopioverN2*j);
		}
	      }
	    }
	  static vis_d64 mult4x4(vis_d64 mult1,vis_d64 mult2)
	    { 
	      vis_d64 prodhihi,prodhilo,prodlohi,prodlolo;
	      vis_d64 prodhi,prodlo,product;
	      vis_f32  mult1hi,mult1lo,mult2hi,mult2lo;
	      vis_f32  packhi,packlo;

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
	  static vis_f32 mult2x2(vis_f32 mult1,vis_f32 mult2)
	    {
	      vis_d64 resultu,resultl,result;
	      vis_f32 product;

	      resultu = vis_fmuld8sux16(mult1,mult2);
	      resultl = vis_fmuld8ulx16(mult1,mult2);
	      result = vis_fpadd32(resultu,resultl);
	      return product = vis_fpackfix(result);
	    }
	  static void reorderfft(vis_s16 *s1,vis_s16 *s2, int nn)
	    {
	      unsigned long n,j,i,m;
	      vis_s16 tempr;

	      n=nn<<1;
	      j=1;
	      for(i=1;i<n;i+=2){
		if(j>i){
		  SWAP(s1[(j-1)/2],s1[(i-1)/2]);
		  SWAP(s2[(j-1)/2],s2[(i-1)/2]);
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
	  vis_d64 *rein,*imin;
	  vis_s16 *cossarray,*sinsarray;
	}
	constructor {
	  rein = imin = 0;
	  cossarray = sinsarray =  0;
	}
	destructor {
	  free(rein);
	  free(imin);
	  free(cossarray);
	  free(sinsarray);
	}
	begin {
	  //allocate memory
	  if (rein) free(rein);
	  if (imin) free(imin);
	  if (cossarray) free(cossarray);
	  if (sinsarray) free(sinsarray);
	  rein = (double *)
		memalign(sizeof(double),sizeof(double)*sizeoffft/4);
	  imin = (double *)
	  	memalign(sizeof(double),sizeof(double)*sizeoffft/4);
	  cossarray = (short *) memalign(sizeof(vis_d64),sizeof(vis_s16)*(sizeoffft-4));
	  sinsarray = (short *) memalign(sizeof(vis_d64),sizeof(vis_s16)*(sizeoffft-4));

	  calcTwSinCos(sinsarray,cossarray,orderfft,sizeoffft);	  
	}
	setup {
          realIn.setSDFParams(sizeoffft/4,sizeoffft/4-1);
          imagIn.setSDFParams(sizeoffft/4,sizeoffft/4-1);
          realOut.setSDFParams(sizeoffft/4,sizeoffft/4-1);
	  imagOut.setSDFParams(sizeoffft/4,sizeoffft/4-1);
	}
	go {	
	  vis_d64 reindtmp,imagindtmp,xtcd,xtsd,ytsd,ytcd,t0,t1,*twC,*twS;
	  vis_f32 reinftmp,imaginftmp,xtcf,ytsf,xtsf,ytcf;
	  vis_f32 *Cf,*Sf,*splitf_rein,*splitf_imin,C0,S0;
	  int     N,N1,N2,i,j,k,l,genindex;
	  vis_s16 reinstmp,imaginstmp,*splits_rein,*splits_imin;

	  vis_write_gsr(8);
	  splitf_rein = (vis_f32*)rein;
	  splitf_imin = (vis_f32*)imin;
	  splits_rein = (vis_s16*)rein;
	  splits_imin = (vis_s16*)imin;
	  twC = (vis_d64*) cossarray;
	  twS = (vis_d64*) sinsarray;
	  /*read in the input*/
	  for(genindex=0;genindex<sizeoffft/4;genindex++){
	    rein[genindex] = double(realIn%genindex);
	    imin[genindex] = double(imagIn%genindex);
	  }
	  /*
	   * first stages of fft (order of fft minus
	   * last two)
	   */   
	  N = sizeoffft;
	  N2 = N;
	  for(k = 0;k < orderfft-2;k++){
	    N1 = N2;
	    N2 = N2/2;
	    for(j = 0;j < N2/4;j++){
	      t0 = *twC++;
	      t1 = *twS++;
	      for(i = j;i < N/4;i += N1/4){
		l = i + N2/4;
		reindtmp = vis_fpsub16(rein[i], rein[l]);
		rein[i] = vis_fpadd16(rein[i], rein[l]);
		imagindtmp = vis_fpsub16(imin[i], imin[l]);
		imin[i] = vis_fpadd16(imin[i], imin[l]);
		xtcd = mult4x4(reindtmp,t0);
		xtsd = mult4x4(reindtmp,t1);
		ytsd = mult4x4(imagindtmp,t1);
		ytcd = mult4x4(imagindtmp,t0);
		rein[l] = vis_fpsub16(xtcd, ytsd);
		imin[l] = vis_fpadd16(xtsd, ytcd);
	      }
	    }
	  }
	  /*second to last stage of the fft*/
	  C0 = vis_to_float(0x7fff<<16|0x0000);
	  S0 = vis_to_float(0x0000<<16|0x8000);
	  j = 0;
	  for(i = 0;i < N/4;i++){
	    reinftmp = vis_fpsub16s(splitf_rein[j], splitf_rein[j+1]);
	    imaginftmp = vis_fpsub16s(splitf_imin[j], splitf_imin[j+1]);
	    splitf_rein[j] = vis_fpadd16s(splitf_rein[j + 1], splitf_rein[j]);
	    splitf_imin[j] = vis_fpadd16s(splitf_imin[j + 1], splitf_imin[j]);
	    xtcf = mult2x2(reinftmp, C0);
	    xtsf = mult2x2(reinftmp, S0);
	    ytsf = mult2x2(imaginftmp, S0);
	    ytcf = mult2x2(imaginftmp, C0);
	    splitf_rein[j+1] = vis_fpsub16s(xtcf, ytsf);
	    splitf_imin[j+1] = vis_fpadd16s(xtsf, ytcf);
	    j += 2;
	  }	
	  /*last stage of the fft*/
	  i = 0;
	  for(j = 0;j < N/2;j++){
	    reinstmp = splits_rein[i] - splits_rein[i+1];
	    imaginstmp = splits_imin[i] - splits_imin[i+1];
	    splits_rein[i] = splits_rein[i] + splits_rein[i + 1];
	    splits_imin[i] = splits_imin[i] + splits_imin[i + 1];
	    splits_rein[i+1] = reinstmp;
	    splits_imin[i+1] = imaginstmp;
	    i += 2;
	  }	
	  reorderfft(splits_rein,splits_imin,sizeoffft);
	  /*output the results*/
	  for(i=0;i<sizeoffft/4;i++){
	    realOut%i << rein[i];
	    imagOut%i << imin[i];
	  }
	}
}
