defstar {
	name { VISFFTCx }
	domain { CGC }
	version { $Id$ }
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
	}
	codeblock(quadmult) {
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
	}
	codeblock(doublemult){
	  static float mult2x2(float mult1,float mult2)
	    {
	      double resultu,resultl,result;
	      float product;

	      resultu = vis_fmuld8sux16(mult1,mult2);
	      resultl = vis_fmuld8ulx16(mult1,mult2);
	      result = vis_fpadd32(resultu,resultl);
	      return product = vis_fpackfix(result);
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
	initCode {
	  addInclude("<vis_proto.h>");
	  addInclude("<math.h>");
	  addProcedure(calculateSinandCos);
	  addProcedure(quadmult);
	  addProcedure(doublemult);
	  addProcedure(bitreverse);
	  addDeclaration(mainDecl);
	  addCode(initialize);
	}
	codeblock(localDecl) {
	  double ExpofW,reindtmp,imagindtmp;
	  double xtcd,xtsd,ytsd,ytcd;
	  float xtcf,ytsf,xtsf,ytcf;
	  float reinftmp,imaginftmp;
	  float *splitf_rein,*splitf_imin;
	  float *Cf,*Sf;
	  int N,N1,N2,i,j,k,l;
	  short *splits_rein,*splits_imin,reinstmp,imaginstmp;
	}	
	codeblock(fft) {       
	  splitf_rein = (float*)$starSymbol(rein);
	  splitf_imin = (float*)$starSymbol(imin);
	  splits_rein = (short*)$starSymbol(rein);
	  splits_imin = (short*)$starSymbol(imin);
	  /*read in the input*/
	  for(i=0;i<$val(sizeoffft)/4;i++){
	    $starSymbol(rein)[i] = (double) $ref2(realIn,i);
	    $starSymbol(imin)[i] = (double) $ref2(imagIn,i);
	  }
	  /*first stages of fft (order of fft minus last two)*/
	  N=$val(sizeoffft);
	  N2=N;
	  for(k=0;k<$val(orderfft)-2;k++){
	    N1=N2;
	    N2=N2/2;
	    ExpofW=$starSymbol(twopi)/N1;
	    for(j=0;j<N2/4;j++){
	      calcTwSinCos($starSymbol(Twsine),$starSymbol(Twcosine),ExpofW,N2);
	      for(i=j;i<N/4;i+=N1/4){
		l=i+N2/4;
		reindtmp=vis_fpsub16($starSymbol(rein)[l],$starSymbol(rein)[i]);
		$starSymbol(rein)[l] =
		  vis_fpadd16($starSymbol(rein)[i],$starSymbol(rein)[l]);
		imagindtmp=vis_fpsub16($starSymbol(imin)[l],$starSymbol(imin)[i]);
		$starSymbol(imin)[l] =
		  vis_fpadd16($starSymbol(imin)[i],$starSymbol(imin)[l]);
		xtcd=mult4x4(reindtmp,$starSymbol(Twcosine)[j]);
		xtsd=mult4x4(reindtmp,$starSymbol(Twsine)[j]);
		ytsd=mult4x4(imagindtmp,$starSymbol(Twsine)[j]);
		ytcd=mult4x4(imagindtmp,$starSymbol(Twcosine)[j]);
		$starSymbol(rein)[i]=vis_fpsub16(xtcd,ytsd);
		$starSymbol(imin)[i]=vis_fpadd16(xtsd,ytcd);
	      }
	    }
	  }
	  /*second to last stage of the fft*/
	  N1=N2;
	  N2=N2/2;
	  ExpofW=$starSymbol(twopi)/N1;
	  calcTwSinCos($starSymbol(Twsine),$starSymbol(Twcosine),ExpofW,N2);
	  Cf=(float *) $starSymbol(Twcosine);
	  Sf=(float *) $starSymbol(Twsine);
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
	  /*last stage of the fft*/
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


