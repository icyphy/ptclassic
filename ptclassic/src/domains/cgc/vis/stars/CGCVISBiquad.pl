defstar {
	name { VISBiquad }
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
	  An IIR Biquad filter.
	}
	input {
	  name { signalIn }
	  type { float }
	  desc { Input float type }
	}	
	output {	
	  name { signalOut }
	  type { float }
	  desc { Output float type }
	}
	defstate {
	  name {numtaps}
	  type {floatarray}
	  default {
	".067455 .135 .067455"
	  }
	  desc { Filter tap numerator values (n0+n1*z^-1+n2*z^-2). }
	}		
	defstate {
	  name {dentaps}
	  type {floatarray}
	  default {
	"-1.1430 .41280"
	  }
	  desc { Filter tap denominator values (1+d1*z^-1+d2*z^-2). }
	}
	defstate {
	  name {scalefactor}
	  type {int}
	  default {"2"}
	  desc { 2^scalefactor is used to scale down the magnitude
		   of the numerator and denominator coefficients
		   between 0 and 1. }
	  attributes{ A_CONSTANT|A_SETTABLE }
	}
      	defstate {
	  name { scaledata }
	  type { float }
	  default { "32767.0" }
	  desc { Filter tap scale }
	  attributes { A_CONSTANT|A_SETTABLE }
	}
      	defstate {
	  name { scaletaps }
	  type { float }
	  default { "32767.0/16" }
	  desc { Filter tap scale }
	  attributes { A_CONSTANT|A_SETTABLE }
	}
	setup {
          signalIn.setSDFParams(1,2);
	}
	code {
#define NUMPACK (4)
	}
	codeblock(quadmult){
	  static vis_d64 mult4x4(vis_d64 mult1,vis_d64 mult2)
	    {
	      vis_d64 upper,lower,prod;

	      upper = vis_fmul8sux16(mult1,mult2);
	      lower = vis_fmul8ulx16(mult1,mult2);
	      return prod = vis_fpadd16(upper,lower);
	    }
	}
	codeblock(settaps){
	  static void settaps(vis_d64* taps1, vis_s16* taps,vis_d64 betatop,vis_d64
			      betabott,vis_s16 scaledown){

	    vis_d64 t0,t1,t2,t3;
	    vis_s16 stmp[8],*splitsh;

	    vis_alignaddr(0,6);

	    taps[0] = 0;
	    taps[1] = 0;
	    taps[2] = (vis_s16)$val(scaletaps)/scaledown*taps1[4];
	    taps[3] = (vis_s16)$val(scaletaps)/scaledown*taps1[3];
	    taps[4] = (vis_s16)$val(scaletaps)/scaledown*taps1[2];
	    taps[5] = 0;
	    taps[6] = 0;
	    taps[7] = 0;
	    taps[8] = 0;
	    taps[9] = 0;
	    taps[10] = (vis_s16)$val(scaletaps)/scaledown*(-(taps1[4]*taps1[0]));
	    taps[11] = (vis_s16)$val(scaletaps)/scaledown*(taps1[4]-(taps1[3]*taps1[0]));
	    taps[12] = (vis_s16)$val(scaletaps)/scaledown*(taps1[3]-(taps1[2]*taps1[0]));
	    taps[13] = (vis_s16)$val(scaletaps)/scaledown*taps1[2];
	    taps[14] = 0;
	    taps[15] = 0;
	    taps[16] = 0;
	    taps[17] = 0;
	    taps[18] =(vis_s16)$val(scaletaps)/scaledown *
	      (taps1[4]*taps1[0]*taps1[0]-taps1[4]*taps1[1]);
	    taps[19] =(vis_s16)$val(scaletaps)/scaledown *
	      (taps1[3]*taps1[0]*taps1[0]-taps1[4]*taps1[0]-taps1[3]*taps1[1]);
	    taps[20] =(vis_s16)$val(scaletaps)/scaledown *
	      (taps1[2]*taps1[0]*taps1[0]-taps1[3]*taps1[0]-taps1[2]*taps1[1]+taps1[4]);
	    taps[21] = (vis_s16)$val(scaletaps)/scaledown * (taps1[3]-(taps1[2]*taps1[0]));
	    taps[22] = (vis_s16)$val(scaletaps)/scaledown * taps1[2];
	    taps[23] = 0;
	    taps[24] = 0;
	    taps[25] = 0;
	    taps[26] = (vis_s16)$val(scaletaps)/scaledown *
	      (2*taps1[4]*taps1[0]*taps1[1]-taps1[4]*taps1[0]*taps1[0]*taps1[0]);
	    taps[27] = (vis_s16)$val(scaletaps)/scaledown *
	      (taps1[4]*taps1[0]*taps1[0]-taps1[4]*taps1[1] -
	       taps1[3]*taps1[0]*taps1[0]*taps1[0]+2*taps1[3]*taps1[0]*taps1[1]);
	    taps[28] = (vis_s16)$val(scaletaps)/scaledown *
	      (taps1[3]*taps1[0]*taps1[0] -
	       taps1[2]*taps1[0]*taps1[0]*taps1[0] -
	       taps1[3]*taps1[1]-taps1[4]*taps1[0]+2*taps1[2]*taps1[0]*taps1[1]);
	    taps[29] = (vis_s16)$val(scaletaps)/scaledown *
	      (taps1[4]-taps1[3]*taps1[0]-taps1[2]*taps1[1]+taps1[2]*taps1[0]*taps1[0]);
	    taps[30] = (vis_s16)$val(scaletaps)/scaledown*(taps1[3]-(taps1[2]*taps1[0]));
	    taps[31] = (vis_s16)$val(scaletaps)/scaledown*taps1[2];
	    
	    stmp[0] = (vis_s16)$val(scaletaps)/scaledown*(-taps1[1]);
	    stmp[1] = (vis_s16)$val(scaletaps)/scaledown*(-taps1[0]);
	    stmp[2] = (vis_s16)$val(scaletaps)/scaledown*(taps1[0]*taps1[1]);
	    stmp[3] = (vis_s16)$val(scaletaps)/scaledown*(taps1[0]*taps1[0]-taps1[1]);
	    stmp[4] = (vis_s16)$val(scaletaps)/scaledown*(taps1[1]*taps1[1]-taps1[0]*taps1[0]*taps1[1]);
	    stmp[5] = (vis_s16)$val(scaletaps)/scaledown*(2*taps1[0]*taps1[1]-taps1[0]*taps1[0]*taps1[0]);
	    stmp[6] = (vis_s16)$val(scaletaps)/scaledown *
	      (taps1[0]*taps1[0]*taps1[0]*taps1[1]-2*taps1[0]*taps1[1]*taps1[1]);
	    stmp[7] = (vis_s16)$val(scaletaps)/scaledown *
	      (taps1[0]*taps1[0]*taps1[0]*taps1[0]-3*taps1[0]*taps1[0]*taps1[1]+taps1[1]*taps1[1]);

	    betatop = vis_fzero();
	    betabott = vis_fzero();
	    splitsh = (vis_s16 *) stmp;
	    t0 = vis_ld_u16(splitsh+0);
	    t1 = vis_ld_u16(splitsh+1);
	    t2 = vis_ld_u16(splitsh+2);
	    t3 = vis_ld_u16(splitsh+3);
	    betatop = vis_faligndata(t3,betatop);
	    betatop = vis_faligndata(t2,betatop);
	    betatop = vis_faligndata(t1,betatop);
	    betatop = vis_faligndata(t0,betatop);
	    t0 = vis_ld_u16(splitsh+4);
	    t1 = vis_ld_u16(splitsh+5);
	    t2 = vis_ld_u16(splitsh+6);
	    t3 = vis_ld_u16(splitsh+7);
	    betabott = vis_faligndata(t3,betabott);
	    betabott = vis_faligndata(t2,betabott);
	    betabott = vis_faligndata(t1,betabott);
	    betabott = vis_faligndata(t0,betabott);
	  }
	}
	codeblock(mainDecl) {
	  vis_s16* $starSymbol(taps) = (vis_s16*)memalign(sizeof(vis_d64),sizeof(vis_s16)*32);
	  vis_d64  $starSymbol(betabott),$starSymbol(betatop),$starSymbol(taps1)[5];
	  vis_s16  $starSymbol(scaledown);
	}
	codeblock(initialize) {
	  $starSymbol(taps1)[0] = $ref2(dentaps,0);
	  $starSymbol(taps1)[1] = $ref2(dentaps,1);
	  $starSymbol(taps1)[2] = $ref2(numtaps,0);
	  $starSymbol(taps1)[3] = $ref2(numtaps,1);
	  $starSymbol(taps1)[4] = $ref2(numtaps,2); 
	  $starSymbol(scaledown) = (short) 1 << $val(scalefactor);
	  settaps($starSymbol(taps1),$starSymbol(taps),$starSymbol(betatop),$starSymbol(betabott),$starSymbol(scaledown));
      	}
	initCode {
	  addInclude("<vis_proto.h>");
	  addInclude("<vis_types.h>");
	  addDeclaration(mainDecl);
	  addProcedure(quadmult,"mult4");
	  addProcedure(settaps,"taps");
	  addCode(initialize);
	}
	codeblock(localDecl) {
	  vis_d64 repeatstate,accumquad0,accumquad1,accumquad2,accumquad3;
	  vis_d64 quad0,quad1,quad2,quad3,statetop,statebott;
	  vis_d64 in0,in1,*indextaps;
	  vis_f32 accumpair0hi,accumpair0lo,accumpair1hi,accumpair1lo;
	  vis_f32 accumpair2hi,accumpair2lo,accumpair3hi,accumpair3lo;
	  vis_f32 accumpair0,accumpair1,accumpair2,accumpair3,currentstate;
	  vis_f32 statetophi,statetoplo,statebotthi,statebottlo;
	  vis_s32 result0,result1,result2,result3;
	  vis_u32 fu,ffu;
	  int i;
	  vis_s16 out0,out1,out2,out3;
	}
	codeblock(filter){
	  currentstate = vis_fzeros();
	  in1 = (vis_d64)$ref2(signalIn,0);
	  in0 = (vis_d64)$ref2(signalIn,1);
	  repeatstate = vis_freg_pair(currentstate,currentstate);
	  statetop = mult4x4(repeatstate,$starSymbol(betatop));
	  statetophi = vis_read_hi(statetop);
	  statetoplo = vis_read_lo(statetop);
	  statebott = mult4x4(repeatstate,$starSymbol(betabott));
	  statebotthi = vis_read_hi(statebott);
	  statebottlo = vis_read_lo(statebott);

	  indextaps = (vis_d64*) $starSymbol(taps);
	  accumquad0 = mult4x4(in0,*indextaps++);
	  quad0 = mult4x4(in1,*indextaps++);
	  accumquad1 = mult4x4(in0,*indextaps++);
	  quad1 = mult4x4(in1,*indextaps++);
	  accumquad2 = mult4x4(in0,*indextaps++);
	  quad2 = mult4x4(in1,*indextaps++);
	  accumquad3 = mult4x4(in0,*indextaps++);
	  quad3 = mult4x4(in1,*indextaps);

	  accumquad0 = vis_fpadd16(accumquad0,quad0);
	  accumpair0hi = vis_read_hi(accumquad0);
	  accumpair0lo = vis_read_lo(accumquad0);
	  accumquad1 = vis_fpadd16(accumquad1,quad1);
	  accumpair1hi = vis_read_hi(accumquad1);
	  accumpair1lo = vis_read_lo(accumquad1);
	  accumquad2 = vis_fpadd16(accumquad2,quad2);
	  accumpair2hi = vis_read_hi(accumquad2);
	  accumpair2lo = vis_read_lo(accumquad2);
	  accumquad3 = vis_fpadd16(accumquad3,quad3);
	  accumpair3hi = vis_read_hi(accumquad3);
	  accumpair3lo = vis_read_lo(accumquad3);

	  accumpair0 = vis_fpadd16s(accumpair0hi,accumpair0lo);
	  accumpair0 = vis_fpadd16s(accumpair0,statetophi);
	  result0 = *((vis_s32*) &accumpair0);
	  out0 = (result0+(result0>>16))<<($val(scalefactor)+1);
	  accumpair1 = vis_fpadd16s(accumpair1hi,accumpair1lo);
	  accumpair1 = vis_fpadd16s(accumpair1,statetoplo);
	  result1 = *((vis_s32*) &accumpair1);
	  out1 = (result1+(result1>>16))<<($val(scalefactor)+1);

	  ffu = out0 << 16 | out1 & 0xffff;
	  accumpair2 = vis_fpadd16s(accumpair2hi,accumpair2lo);
	  accumpair2 = vis_fpadd16s(accumpair2,statebotthi);
	  result2 = *((vis_s32*) &accumpair2);
	  out2 = (result2+(result2>>16))<<($val(scalefactor)+1);
	  accumpair3 = vis_fpadd16s(accumpair3hi,accumpair3lo);
	  accumpair3 = vis_fpadd16s(accumpair3,statebottlo);
	  result3 = *((vis_s32*) &accumpair3);
	  out3 = (result3+(result3>>16))<<($val(scalefactor)+1);
    
	  fu = out2 << 16 | out3 & 0xffff;
	  $ref(signalOut) = vis_to_double(ffu,fu);
	  currentstate = vis_to_float(fu);
	}	
	go {
	  addCode(localDecl);
	  addCode(filter);
	}
	wrapup {
          addCode("free($starSymbol(taps));");
        }
}

