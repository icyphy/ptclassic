defstar {
	name { VISBiquad }
	domain { CGC }
	version { $Id$ }
	author { William Chen and John Reekie}
	copyright {
Copyright (c) 1996-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC Visual Instruction Set library }
	desc { 
An IIR Biquad filter.  In order to take advantage of the 16-bit 
partitioned multiplies, the VIS Biquad reformulates the filtering 
operation to that of a matrix operation (Ax=y), where A is a matrix 
calculated from the taps, x is an input vector, and y is an output vector.
The matrix A is first calculated by substituting the biquad equation 
y[n] = -a*y[n-1]-b*y[n-2]+c*x[n]+d*x[n-1]+e*x[n-2] into y[n-1], y[n-2], 
and y[n-3].  The matrix A is then multiplied with the 16-bit partitioned 
input vector.  The final result is accumulated in four 16-bit fixed point 
numbers which are concatenated into a single 64-bit float particle.
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
	  default { ".067455 .135 .067455" }
	  desc { Filter tap numerator values (n0+n1*z^-1+n2*z^-2). }
	}		
	defstate {
	  name {dentaps}
	  type {floatarray}
	  default { "-1.1430 .41280" }
	  desc { Filter tap denominator values (1+d1*z^-1+d2*z^-2). }
	}
	defstate {
	  name {scalefactor}
	  type {int}
	  default {"2"}
	  desc {
2^scalefactor is used to scale down the magnitude of the numerator and
denominator coefficients between 0 and 1.
	  }
	  attributes{ A_CONSTANT|A_SETTABLE }
	}
      	defstate {
	  name { scaletaps }
	  type { float }
	  default { "32767.0" }
	  desc { Filter tap scale }
	  attributes { A_CONSTANT|A_SETTABLE }
	}
	setup {
          signalIn.setSDFParams(1,1);
	}
	code {
#define NUMPACK (4)
	}
	codeblock(quadmult){
vis_d64 $sharedSymbol(CGCVISBiquad,mult4x4)(vis_d64 mult1,vis_d64 mult2) {
	vis_d64 upper = vis_fmul8sux16(mult1,mult2);
	vis_d64 lower = vis_fmul8ulx16(mult1,mult2);
	return vis_fpadd16(upper,lower);
}
	}
	codeblock(settaps){
void $sharedSymbol(CGCVISBiquad,settaps)(vis_d64* filtertaps, vis_s16* tapmatrix,
		vis_d64* betatop, vis_d64* betabott, vis_s16 scaledown) {
          vis_d64 t0, t1, t2, t3, top, bottom;
	  vis_s16 statemem[8];

	  vis_alignaddr(0,6);

	  tapmatrix[0] = 0;
	  tapmatrix[1] = 0;
	  tapmatrix[2] = (vis_s16)$val(scaletaps)/scaledown*filtertaps[4];
	  tapmatrix[3] = (vis_s16)$val(scaletaps)/scaledown*filtertaps[3];
	  tapmatrix[4] = (vis_s16)$val(scaletaps)/scaledown*filtertaps[2];
	  tapmatrix[5] = 0;
	  tapmatrix[6] = 0;
	  tapmatrix[7] = 0;
	  tapmatrix[8] = 0;
	  tapmatrix[9] = 0;
	  tapmatrix[10] = (vis_s16)$val(scaletaps)/scaledown*(-(filtertaps[4]*filtertaps[0]));
	  tapmatrix[11] =
	    (vis_s16)$val(scaletaps)/scaledown*(filtertaps[4]-(filtertaps[3]*filtertaps[0]));
	  tapmatrix[12] =
	    (vis_s16)$val(scaletaps)/scaledown*(filtertaps[3]-(filtertaps[2]*filtertaps[0]));
	  tapmatrix[13] = (vis_s16)$val(scaletaps)/scaledown*filtertaps[2];
	  tapmatrix[14] = 0;
	  tapmatrix[15] = 0;
	  tapmatrix[16] = 0;
	  tapmatrix[17] = 0;
	  tapmatrix[18] = (vis_s16)$val(scaletaps)/scaledown *
	    (filtertaps[4]*filtertaps[0]*filtertaps[0]-filtertaps[4]*filtertaps[1]);
	  tapmatrix[19] = (vis_s16)$val(scaletaps)/scaledown *
	    (filtertaps[3]*filtertaps[0]*filtertaps[0]-filtertaps[4]*filtertaps[0]-filtertaps[3]*filtertaps[1]);
	  tapmatrix[20] = (vis_s16)$val(scaletaps)/scaledown *
	    (filtertaps[2]*filtertaps[0]*filtertaps[0] -
	     filtertaps[3]*filtertaps[0]-filtertaps[2]*filtertaps[1]+filtertaps[4]);
	  tapmatrix[21] = (vis_s16)$val(scaletaps)/scaledown *
	    (filtertaps[3]-(filtertaps[2]*filtertaps[0]));
	  tapmatrix[22] = (vis_s16)$val(scaletaps)/scaledown * filtertaps[2];
	  tapmatrix[23] = 0;
	  tapmatrix[24] = 0;
	  tapmatrix[25] = 0;
	  tapmatrix[26] = (vis_s16)$val(scaletaps)/scaledown *
	    (2*filtertaps[4]*filtertaps[0]*filtertaps[1] -
	     filtertaps[4]*filtertaps[0]*filtertaps[0]*filtertaps[0]);
	  tapmatrix[27] = (vis_s16)$val(scaletaps)/scaledown *
	    (filtertaps[4]*filtertaps[0]*filtertaps[0]-filtertaps[4]*filtertaps[1] -
	     filtertaps[3]*filtertaps[0]*filtertaps[0]*filtertaps[0]+2*filtertaps[3]*filtertaps[0]*filtertaps[1]);
	  tapmatrix[28] = (vis_s16)$val(scaletaps)/scaledown *
	    (filtertaps[3]*filtertaps[0]*filtertaps[0] -
	     filtertaps[2]*filtertaps[0]*filtertaps[0]*filtertaps[0] -
	     filtertaps[3]*filtertaps[1]-filtertaps[4]*filtertaps[0]+2*filtertaps[2]*filtertaps[0]*filtertaps[1]);
	  tapmatrix[29] = (vis_s16)$val(scaletaps)/scaledown *
	    (filtertaps[4]-filtertaps[3]*filtertaps[0] -
	     filtertaps[2]*filtertaps[1]+filtertaps[2]*filtertaps[0]*filtertaps[0]);
	  tapmatrix[30] = (vis_s16)$val(scaletaps)/scaledown*(filtertaps[3]-(filtertaps[2]*filtertaps[0]));
	  tapmatrix[31] = (vis_s16)$val(scaletaps)/scaledown*filtertaps[2];
	    
	  statemem[0] = (vis_s16)$val(scaletaps)/scaledown*(-filtertaps[1]);
	  statemem[1] = (vis_s16)$val(scaletaps)/scaledown*(-filtertaps[0]);
	  statemem[2] = (vis_s16)$val(scaletaps)/scaledown*(filtertaps[0]*filtertaps[1]);
	  statemem[3] = (vis_s16)$val(scaletaps)/scaledown*(filtertaps[0]*filtertaps[0]-filtertaps[1]);
	  statemem[4] = (vis_s16)$val(scaletaps)/scaledown *
	    (filtertaps[1]*filtertaps[1]-filtertaps[0]*filtertaps[0]*filtertaps[1]);
	  statemem[5] = (vis_s16)$val(scaletaps)/scaledown *
	    (2*filtertaps[0]*filtertaps[1]-filtertaps[0]*filtertaps[0]*filtertaps[0]);
	  statemem[6] = (vis_s16)$val(scaletaps)/scaledown *
	    (filtertaps[0]*filtertaps[0]*filtertaps[0]*filtertaps[1]-2*filtertaps[0]*filtertaps[1]*filtertaps[1]);
	  statemem[7] = (vis_s16)$val(scaletaps)/scaledown *
	    (filtertaps[0]*filtertaps[0]*filtertaps[0]*filtertaps[0]
	     -3*filtertaps[0]*filtertaps[0]*filtertaps[1]+filtertaps[1]*filtertaps[1]);

	  top = vis_fzero();
	  bottom = vis_fzero();
	  t0 = vis_ld_u16(statemem+0);
	  t1 = vis_ld_u16(statemem+1);
	  t2 = vis_ld_u16(statemem+2);
	  t3 = vis_ld_u16(statemem+3);
	  top = vis_faligndata(t3,top);
	  top = vis_faligndata(t2,top);
	  top = vis_faligndata(t1,top);
	  top = vis_faligndata(t0,top);
	  t0 = vis_ld_u16(statemem+4);
	  t1 = vis_ld_u16(statemem+5);
	  t2 = vis_ld_u16(statemem+6);
	  t3 = vis_ld_u16(statemem+7);
	  bottom = vis_faligndata(t3,bottom);
	  bottom = vis_faligndata(t2,bottom);
	  bottom = vis_faligndata(t1,bottom);
	  bottom = vis_faligndata(t0,bottom);
	  *betatop = top;
	  *betabott = bottom;
}	
	}

	codeblock(mainDecl) {
	  $starSymbol(tapmatrix) = (vis_s16*)memalign(sizeof(vis_d64),sizeof(vis_s16)*32);
	}
	
	codeblock(globaldec) {
	  vis_s16* $starSymbol(tapmatrix) = NULL;
	  vis_d64  $starSymbol(betabott), $starSymbol(betatop);
	  vis_f32  $starSymbol(currentstate);
	  vis_s16  $starSymbol(scaledown);
	}

	codeblock(initialize) {
	  $starSymbol(filtertaps)[0] = $ref2(dentaps,0);
	  $starSymbol(filtertaps)[1] = $ref2(dentaps,1);
	  $starSymbol(filtertaps)[2] = $ref2(numtaps,0);
	  $starSymbol(filtertaps)[3] = $ref2(numtaps,1);
	  $starSymbol(filtertaps)[4] = $ref2(numtaps,2); 
	  $starSymbol(scaledown) = (short) 1 << $val(scalefactor);
	  $sharedSymbol(CGCVISBiquad,settaps)($starSymbol(filtertaps),$starSymbol(tapmatrix),&$starSymbol(betatop),&$starSymbol(betabott),$starSymbol(scaledown));
	  $starSymbol(currentstate) = vis_fzeros();
      	}

	initCode {
	  addInclude("<vis_proto.h>");
	  addInclude("<vis_types.h>");
	  addGlobal("vis_d64 $starSymbol(filtertaps)[5];");
	  addGlobal(globaldec);
	  addCode(mainDecl);
	  addProcedure(quadmult, "CGCVISBiquad_mult4x4");
	  addProcedure(settaps, "CGCVISBiquad_settaps");
	  addCode(initialize);
	}

	codeblock(localDecl) {
	  vis_d64 repeatstate,accumquad0,accumquad1,accumquad2,accumquad3;
	  vis_d64 quad0,quad1,quad2,quad3,statetop,statebott;
	  vis_d64 in0,in1,*indextaps;
	  vis_f32 accumpair0hi,accumpair0lo,accumpair1hi,accumpair1lo;
	  vis_f32 accumpair2hi,accumpair2lo,accumpair3hi,accumpair3lo;
	  vis_f32 accumpair0,accumpair1,accumpair2,accumpair3;
	  vis_f32 statetophi,statetoplo,statebotthi,statebottlo;
	  vis_s32 result0,result1,result2,result3;
	  vis_u32 fu,ffu;
	  vis_s16 out0,out1,out2,out3;
	}

	codeblock(filter){
	  /*set up inputs*/
	  in1 = (vis_d64)$ref2(signalIn,0);
	  in0 = (vis_d64)$ref2(signalIn,1);
	  
	  repeatstate = vis_freg_pair($starSymbol(currentstate),$starSymbol(currentstate));
	  statetop = $sharedSymbol(CGCVISBiquad,mult4x4)(repeatstate,$starSymbol(betatop));
	  statetophi = vis_read_hi(statetop);
	  statetoplo = vis_read_lo(statetop);
	  statebott = $sharedSymbol(CGCVISBiquad,mult4x4)(repeatstate,$starSymbol(betabott));
	  statebotthi = vis_read_hi(statebott);
	  statebottlo = vis_read_lo(statebott);
	  
	  indextaps = (vis_d64*) $starSymbol(tapmatrix);
	  accumquad0 = $sharedSymbol(CGCVISBiquad,mult4x4)(in0,*indextaps++);
	  quad0 = $sharedSymbol(CGCVISBiquad,mult4x4)(in1,*indextaps++);
	  accumquad1 = $sharedSymbol(CGCVISBiquad,mult4x4)(in0,*indextaps++);
	  quad1 = $sharedSymbol(CGCVISBiquad,mult4x4)(in1,*indextaps++);
	  accumquad2 = $sharedSymbol(CGCVISBiquad,mult4x4)(in0,*indextaps++);
	  quad2 = $sharedSymbol(CGCVISBiquad,mult4x4)(in1,*indextaps++);
	  accumquad3 = $sharedSymbol(CGCVISBiquad,mult4x4)(in0,*indextaps++);
	  quad3 = $sharedSymbol(CGCVISBiquad,mult4x4)(in1,*indextaps++);

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
	  $starSymbol(currentstate) = vis_to_float(fu);
	}

	go {
	  addCode(localDecl);
	  addCode(filter);
	}

	wrapup {
	  addCode("free($starSymbol(tapmatrix));");
        }
}
