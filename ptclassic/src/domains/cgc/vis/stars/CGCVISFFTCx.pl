defstar {
	name { VISFFTCx }
	domain { CGC }
	version { $Id$ }
	author { William Chen }
	copyright {
Copyright (c) 1996-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC Visual Instruction Set library }
	desc { 
A radix-2 FFT of a complex input.  The radix-2 decimation-in-time 
decomposes the overall FFT operation into a series of smaller FFT 
operations.  The smallest operation is the "FFT butterfly" which consists 
of a single addition and subtraction.  Graphically, the full
decomposition can be viewed as N stages of FFT butterflies with twiddle
factors between each of the stages.  One standard implementation is to use 
three nested for loops to calculate the FFT.  The innermost loop calculates 
all the butterflies and performs twiddle factor multiplcations within a particular stage; 
the next outer loop calculates the twiddle factors; and the outermost loop steps 
through all the stages.  In order to take advantage of the 16-bit partitioned 
multiplications and additions, the basic operation of the VIS FFT is actually doing 
four "FFT butterflies" at once.  The implementation is similar to the standard 
three nested for loops, but the last two stages are separated out. In order 
to avoid packing and unpacking, the basic operation of the last two stages 
switches from four to two to eventually just one "FFT butterfly".  After the FFT 
is taken, the order of the sequence is bit-reversed.
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

	codeblock(calculateSinandCos) {
void $sharedSymbol(CGCVISFFTCx,calcTwSinCos)
                  (vis_s16 *sinsarray,vis_s16 *cossarray,int order,int size){
	vis_d64 twopioverN2;
	vis_s16* indexcounts0 = cossarray;
	vis_s16* indexcounts1 = sinsarray;
	int i = 0;
	int N2 = size;
	int maxi = order - 2;

	for(i = 0; i < maxi; i++) {
	  int j;
	  twopioverN2 = M_TWO_PI/N2;
	  N2 = N2/2;
	  for(j = 0; j < N2; j++){
	    *indexcounts0++=(vis_s16)$val(scale) * cos(twopioverN2 * j);
	    *indexcounts1++=(vis_s16)(-$val(scale)) * sin(twopioverN2 * j);
	  }
	}
}
	}
	codeblock(quadmult) {
vis_d64 $sharedSymbol(CGCVISFFTCx,mult4x4d)(vis_d64 mult1,vis_d64 mult2) { 
	vis_d64 prodhihi, prodhilo, prodlohi, prodlolo;
	vis_d64 prodhi, prodlo, product;
	vis_f32 packhi, packlo;

	vis_f32 mult1hi = vis_read_hi(mult1);
	vis_f32 mult1lo = vis_read_lo(mult1);
	vis_f32 mult2hi = vis_read_hi(mult2);
	vis_f32 mult2lo = vis_read_lo(mult2);

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
vis_f32 $sharedSymbol(CGCFFTCx,mult2x2)(vis_f32 mult1, vis_f32 mult2) {
	vis_d64 resultu,resultl,result;
	vis_f32 product;

	resultu = vis_fmuld8sux16(mult1,mult2);
	resultl = vis_fmuld8ulx16(mult1,mult2);
	result = vis_fpadd32(resultu,resultl);
	return product = vis_fpackfix(result);
}
	}
	codeblock(bitreverse) {
#define SWAP(a,b,t) (t)=(a); (a)=(b); (b)=(t)

void $sharedSymbol(CGCVISFFTFx,reorderfft)(vis_s16* s1, vis_s16* s2, int nn) {
	unsigned long n,j,i,m;
	vis_s16 tempr;

	n= nn << 1;
	j = 1;
	for (i = 1; i < n; i += 2){
	  if (j > i) {
	    SWAP(s1[(j-1)/2], s1[(i-1)/2], tempr);
	    SWAP(s2[(j-1)/2], s2[(i-1)/2], tempr);
	  }
	  m = n >> 1;
	  while(m >= 2 && j > m){
	    j -= m;
	    m >>= 1;
	  }
	  j += m;
	}
}
	}

	codeblock(globalDecl){
#define M_TWO_PI (2*M_PI)
	}

	codeblock(mainDecl) {
	  vis_d64 *$starSymbol(rein) = (vis_d64 *)
	    memalign(sizeof(vis_d64),sizeof(vis_d64)*$val(sizeoffft)/4);
	  vis_d64 *$starSymbol(imin) = (vis_d64 *)
	    memalign(sizeof(vis_d64),sizeof(vis_d64)*$val(sizeoffft)/4);
	  vis_s16 *$starSymbol(cossarray) = (vis_s16 *)
	    memalign(sizeof(vis_d64),sizeof(vis_s16)*($val(sizeoffft)-4));
	  vis_s16 *$starSymbol(sinsarray) = (vis_s16 *)
	    memalign(sizeof(vis_d64),sizeof(vis_s16)*($val(sizeoffft)-4));
	}
	codeblock(initialize) {
	  /* pre-calculate all the twiddle factors */
	  $sharedSymbol(CGCVISFFTCx,calcTwSinCos)
	    ($starSymbol(sinsarray),$starSymbol(cossarray),$val(orderfft),$val(sizeoffft));
	}
	initCode {
	  addInclude("<vis_proto.h>");
	  addInclude("<vis_types.h>");
	  addInclude("<math.h>");
	  addProcedure(calculateSinandCos, "CGCVISFFTCx_calculateSinandCos");
	  addProcedure(quadmult, "CGCVISFFTCx_quadmult");
	  addProcedure(doublemult, "CGCVISFFTCx_doublemult");
	  addProcedure(bitreverse, "CGCVISFFTCx_bitreverse");
	  addGlobal(globalDecl);
	  addDeclaration(mainDecl);
	  addCode(initialize);
	}
	codeblock(localDecl) {
	  vis_d64 reindtmp,imagindtmp,xtcd,xtsd,ytsd,ytcd,t0,t1,*twC,*twS;
	  vis_f32 reinftmp,imaginftmp,xtcf,ytsf,xtsf,ytcf;
	  vis_f32 *Cf,*Sf,*splitf_rein,*splitf_imin,C0,S0;
	  int     N,N1,N2,i,j,k,l,genindex;
	  vis_s16 reinstmp,imaginstmp,*splits_rein,*splits_imin;
	}
	codeblock(fft) {  
	  vis_write_gsr(8);

	  splitf_rein = (vis_f32*)$starSymbol(rein);
	  splitf_imin = (vis_f32*)$starSymbol(imin);
	  splits_rein = (vis_s16*)$starSymbol(rein);
	  splits_imin = (vis_s16*)$starSymbol(imin);
	  twC = (vis_d64*) $starSymbol(cossarray);
	  twS = (vis_d64*) $starSymbol(sinsarray);
	  /*read in the input*/
	  for(genindex=0;genindex<$val(sizeoffft)/4;genindex++){
	    $starSymbol(rein)[genindex] = (double)$ref2(realIn,genindex);
	    $starSymbol(imin)[genindex] = (double)$ref2(imagIn,genindex);
	  }
	  /*
	   * first stages of fft (order of fft minus
	   * last two)
	   */   
	  N = $val(sizeoffft);
	  N2 = N;
	  for(k = 0;k < $val(orderfft)-2;k++){
	    N1 = N2;
	    N2 = N2/2;
	    for(j = 0;j < N2/4;j++){
	      t0 = *twC++;
	      t1 = *twS++;
	      for(i = j;i < N/4;i += N1/4){
		l = i + N2/4;
		reindtmp = vis_fpsub16($starSymbol(rein)[i], $starSymbol(rein)[l]);
		$starSymbol(rein)[i] = vis_fpadd16($starSymbol(rein)[i],$starSymbol(rein)[l]);
		imagindtmp = vis_fpsub16($starSymbol(imin)[i], $starSymbol(imin)[l]);
		$starSymbol(imin)[i] = vis_fpadd16($starSymbol(imin)[i],$starSymbol(imin)[l]);
		xtcd = $sharedSymbol(CGCVISFFTCx,mult4x4d)(reindtmp,t0);
		xtsd = $sharedSymbol(CGCVISFFTCx,mult4x4d)(reindtmp,t1);
		ytsd = $sharedSymbol(CGCVISFFTCx,mult4x4d)(imagindtmp,t1);
		ytcd = $sharedSymbol(CGCVISFFTCx,mult4x4d)(imagindtmp,t0);
		$starSymbol(rein)[l] = vis_fpsub16(xtcd,ytsd);
		$starSymbol(imin)[l] = vis_fpadd16(xtsd,ytcd);
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
	    xtcf = $sharedSymbol(CGCFFTCx,mult2x2)(reinftmp, C0);
	    xtsf = $sharedSymbol(CGCFFTCx,mult2x2)(reinftmp, S0);
	    ytsf = $sharedSymbol(CGCFFTCx,mult2x2)(imaginftmp, S0);
	    ytcf = $sharedSymbol(CGCFFTCx,mult2x2)(imaginftmp, C0);
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
	  /*reorder the outputs*/
	  $sharedSymbol(CGCVISFFTFx,reorderfft)(splits_rein,splits_imin,$val(sizeoffft));
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
	  free($starSymbol(cossarray));
	  free($starSymbol(sinsarray));	  
	}
	wrapup {
	  addCode(freeit);
	}	
}


