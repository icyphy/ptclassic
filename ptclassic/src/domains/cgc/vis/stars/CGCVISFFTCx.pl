defstar {
	name { QuadFFTCx }
	domain { CGC }
	version { $Date$ $Id$ }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC vis library }
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
	setup {
	  int s = sizeoffft;
          realIn.setSDFParams(s/4,s/4-1);
          imagIn.setSDFParams(s/4,s/4-1);
          realOut.setSDFParams(s/4,s/4-1);
	  imagOut.setSDFParams(s/4,s/4-1);
	}
	codeblock(mainDecl) {
	  double *$starSymbol(rein) = (double *)
	    memalign(sizeof(double),sizeof(double)*$val(sizeoffft)/4);
	  double *$starSymbol(imin) = (double *)
	    memalign(sizeof(double),sizeof(double)*$val(sizeoffft)/4);
	  double *$starSymbol(Twcosine) = (double *)
	    memalign(sizeof(double),sizeof(double)*$val(sizeoffft)/8);
	  double *$starSymbol(Twsine) = (double *)
	    memalign(sizeof(double),sizeof(double)*$val(sizeoffft)/8);
	  double $starSymbol(twopi)=6.28318530717959;
	}
	codeblock(initialize) {
	  vis_write_gsr(8);	
	}
	codeblock(calculateSinandCos) {
	  static void calcTwSinCos(double *$starSymbol(Twsine),double
				   *$starSymbol(Twcosine),double ExpofW,int N2)
	    {
	      double scale = 32767.0;
	      int i;
	      short *indexcount0,*indexcount1;
	      
	      indexcount0 = (short *) $starSymbol(Twcosine);
	      indexcount1 = (short *) $starSymbol(Twsine);
	      for(i=0;i<N2;i++){
		*indexcount0++ = (short) scale*cos(ExpofW*(N2-1-i));
		*indexcount1++ = (short) -scale*sin(ExpofW*(N2-1-i));       
	      }
	    }
	}
	codeblock(quadmult) {
	  static double mult4x4(double mult1,double mult2)
	    { 
	      double mult1hi,mult1lo,mult2hi,mult2lo;
	      double prodhihi,prodhilo,prodlohi,prodlolo;
	      double prodhi,prodlo,product;
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
	}
	codeblock(bitreverse) {
#define SWAP(a, b) tempr=(a); (a)=(b); (b)=tempr

	  static void reorderfft(short *s1,short *s2, int nn)
	    {
	      unsigned long n,j,i,m;
	      short tempr;

	      n=nn<<1;
	      j=1;
	      for(i=1;i<n;i+=2){
		if(j>i){
		  SWAP(s1[nn-1-(j-1)/2],s1[nn-1-(i-1)/2]);
		  SWAP(s2[nn-1-(j-1)/2],s2[nn-1-(i-1)/2]);
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
	codeblock(firststages) {
	  static int firststagesfft(int N,int N2,int order,double*rein,double
				     *imin,double *Twsine,double *Twcosine)
	    {
	      int N1,i,j,k,l;
	      double ExpofW,reindtmp,imagindtmp,xtcd,xtsd,ytsd,ytcd;
	      double twopi=6.28318530717959;

	      for(k=0;k<order-2;k++){
		N1=N2;
		N2=N2/2;
		ExpofW=twopi/N1;
		for(j=0;j<N2/4;j++){
		  calcTwSinCos(Twsine,Twcosine,ExpofW,N2);
		  for(i=j;i<N/4;i+=N1/4){
		    l=i+N2/4;
		    reindtmp=vis_fpsub16(rein[l],rein[i]);
		    rein[l] = vis_fpadd16(rein[i],rein[l]);
		    imagindtmp=vis_fpsub16(imin[l],imin[i]);
		    imin[l] = vis_fpadd16(imin[i],imin[l]);
		    xtcd=mult4x4(reindtmp,Twcosine[j]);
		    xtsd=mult4x4(reindtmp,Twsine[j]);
		    ytsd=mult4x4(imagindtmp,Twsine[j]);
		    ytcd=mult4x4(imagindtmp,Twcosine[j]);
		    rein[i]=vis_fpsub16(xtcd,ytsd);
		    imin[i]=vis_fpadd16(xtsd,ytcd);
		  }
		}
	      }
	      return N2;
	    }
	}
	codeblock(secondtolast) {
	  static void secondlaststagefft(int N1,int N,float *splitf_rein, float
					 *splitf_imin, double CSd,double SCd)
	    {
	      double reinrepeat,iminrepeat;
	      double xtcxts,ytsytc,xtcminusyts,xtsaddytc;
	      float reinftmp,imaginftmp;
	      int i;

	      for(i=0;i<N/2;i+=N1/2){
		reinftmp=vis_fpsub16s(splitf_rein[i+1],splitf_rein[i]);
		imaginftmp=vis_fpsub16s(splitf_imin[i+1],splitf_imin[i]);
		reinrepeat=vis_freg_pair(reinftmp,reinftmp);
		iminrepeat=vis_freg_pair(imaginftmp,imaginftmp);
		splitf_rein[i+1]=vis_fpadd16s(splitf_rein[i+1],splitf_rein[i]);
		splitf_imin[i+1]=vis_fpadd16s(splitf_imin[i+1],splitf_imin[i]);
		xtcxts=mult4x4(reinrepeat,CSd);
		ytsytc=mult4x4(iminrepeat,SCd);
		xtcminusyts=vis_fpsub16(xtcxts,ytsytc);
		xtsaddytc=vis_fpadd16(xtcxts,ytsytc);
		splitf_rein[i]=vis_read_hi(xtcminusyts);
		splitf_imin[i]=vis_read_lo(xtsaddytc);
	      }
	    }
	}
	codeblock(last) {
	  static void laststagefft(int N,short *splits_rein,short *splits_imin)
	    {
	      double reindhi,reindlo,reindsub,reindadd;
	      double imindhi,imindlo,imindsub,imindadd;
	      int i,j;

	      for(j=0;j<N/2;j++){
		i=2*j;
		reindhi=vis_ld_u16((splits_rein+i+1));
		reindlo=vis_ld_u16((splits_rein+i));
		reindsub=vis_fpsub16(reindhi,reindlo);
		reindadd=vis_fpadd16(reindhi,reindlo);
		imindhi=vis_ld_u16((splits_imin+i+1));
		imindlo=vis_ld_u16((splits_imin+i));
		imindsub=vis_fpsub16(imindhi,imindlo);
		imindadd=vis_fpadd16(imindhi,imindlo);
		vis_st_u16(reindadd,(splits_rein+i+1));
		vis_st_u16(imindadd,(splits_imin+i+1));
		vis_st_u16(reindsub,(splits_rein+i));
		vis_st_u16(imindsub,(splits_imin+i));
	      }	
	    }
	}
	initCode {
	  addInclude("<vis_proto.h>");
	  addProcedure(calculateSinandCos);
	  addProcedure(quadmult);
	  addProcedure(bitreverse);
	  addProcedure(firststages);
	  addProcedure(secondtolast);
	  addProcedure(last);
	  addDeclaration(mainDecl);
	  addCode(initialize);
	}
	codeblock(localDecl) {
	  double ExpofW;
	  double CSd,SCd;
	  float *splitf_rein,*splitf_imin;
	  float *Cf,*Sf;
	  int i,N,N1,N2;
	  short *splits_rein,*splits_imin;
	}
	codeblock(fft) {       
	  splitf_rein = (float*)$starSymbol(rein);
	  splitf_imin = (float*)$starSymbol(imin);
	  splits_rein = (short*)splitf_rein;
	  splits_imin = (short*)splitf_imin;

	  /*read in the input*/
	  for(i=0;i<$val(sizeoffft)/4;i++){
	    $starSymbol(rein)[i] = (double) $ref2(realIn,i);
	    $starSymbol(imin)[i] = (double) $ref2(imagIn,i);
	  }
	  /*first stages of fft (order of fft minus last two)*/
	  N=$val(sizeoffft);
	  N2=N;
	  N2=firststagesfft(N,N2,$val(orderfft), $starSymbol(rein),
			    $starSymbol(imin), $starSymbol(Twsine),
			    $starSymbol(Twcosine));
	  /*second to last stage of the fft*/
	  N1=N2;
	  N2=N2/2;
	  ExpofW=$starSymbol(twopi)/N1;
	  calcTwSinCos($starSymbol(Twsine),$starSymbol(Twcosine),ExpofW,N2);
	  Cf=(float *) $starSymbol(Twcosine);
	  Sf=(float *) $starSymbol(Twsine);
	  CSd = vis_freg_pair(*Cf,*Sf);
	  SCd = vis_freg_pair(*Sf,*Cf);
	  secondlaststagefft(N1,N,splitf_rein,splitf_imin,CSd,SCd);
	  /*last stage of the fft*/
	  laststagefft(N,splits_rein,splits_imin);
	  reorderfft(splits_rein,splits_imin,$val(sizeoffft));
	  /*output the results*/
	  for(i=0;i<$val(sizeoffft)/4;i++){
	    $ref2(realOut,i) = $starSymbol(rein)[i];
	    $ref2(imagOut,i) = $starSymbol(imin)[i];
	  }	
	}
	go {
	  addCode(localDecl);
	  addCode(fft);
	}
	codeblock(freeit){
	  free($starSymbol(rein));
	  free($starSymbol(imin));
	  free($starSymbol(Twcosine));
	  free($starSymbol(Twsine));	  
	}
	wrapup {
	  addCode(freeit);
	}	
}


