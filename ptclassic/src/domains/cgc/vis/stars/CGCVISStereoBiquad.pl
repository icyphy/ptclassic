defstar {
	name {VISStereoBiquad}
	domain {CGC}
	version { $Id$ }
	desc {
A two-pole, two-zero parametric digital IIR filter (a biquad).
	}
	author { William Chen}
	copyright {
Copyright (c) 1996-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC VIS library }
	htmldoc {
A two-pole, two-zero IIR filter.
	}
	input {
	  name{input}
	  type{ float }
	}
	output {
	  name{output}
	  type{ float }
	}
        state {
                name {d1}
                type { float }
                default { "-1.1430" }
        }
        state {
                name {d2}
                type { float }
                default { "0.41280" }
        }
        state {
                name {n0}
                type { float }
                default { "0.067455" }
        }
        state {
                name {n1}
                type { float }
                default { "0.135" }
        }
        state {
                name {n2}
                type { float }
                default { "0.067455" }
        }
        state {
                name {scale}
                type { float }
                default { "32767.0" }
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
	codeblock(quadmult) {
vis_f32 $sharedSymbol(CGCVISStereoBiquad,mult2x2d)(vis_f32 mult1,vis_f32 mult2) { 
	vis_d64 prodhi, prodlo, product;

	prodhi = vis_fmuld8sux16(mult1,mult2);
	prodlo = vis_fmuld8ulx16(mult1,mult2);

	product = vis_fpadd32(prodhi,prodlo);
	return vis_fpackfix(product);
}
	}

        codeblock(mainDecl){
	  vis_s16 $starSymbol(filtertaps)[5],$starSymbol(scaledown);
	  vis_f32 $starSymbol(repeatd1),$starSymbol(repeatd2);
	  vis_f32 $starSymbol(repeatn0),$starSymbol(repeatn1),$starSymbol(repeatn2);
	  vis_f32 $starSymbol(repeatstate1),$starSymbol(repeatstate2);
	}
	codeblock(settapDef){
	  $starSymbol(scaledown) = 1<< $val(scalefactor);

	  $starSymbol(filtertaps)[0]=$val(scale)/$starSymbol(scaledown)*$val(d1);
	  $starSymbol(filtertaps)[1]=$val(scale)/$starSymbol(scaledown)*$val(d2);
	  $starSymbol(filtertaps)[2]=$val(scale)/$starSymbol(scaledown)*$val(n0);
	  $starSymbol(filtertaps)[3]=$val(scale)/$starSymbol(scaledown)*$val(n1);
	  $starSymbol(filtertaps)[4]=$val(scale)/$starSymbol(scaledown)*$val(n2);

	  $starSymbol(repeatd1) =
	    vis_to_float($starSymbol(filtertaps)[0]<<16|$starSymbol(filtertaps)[0]&0xffff);
	  $starSymbol(repeatd2) =
	    vis_to_float($starSymbol(filtertaps)[1]<<16|$starSymbol(filtertaps)[1]&0xffff);
	  $starSymbol(repeatn0) =
	    vis_to_float($starSymbol(filtertaps)[2]<<16|$starSymbol(filtertaps)[2]&0xffff);
	  $starSymbol(repeatn1) =
	    vis_to_float($starSymbol(filtertaps)[3]<<16|$starSymbol(filtertaps)[3]&0xffff);
	  $starSymbol(repeatn2) =
	    vis_to_float($starSymbol(filtertaps)[4]<<16|$starSymbol(filtertaps)[4]&0xffff);
	}
        initCode{
	  addInclude("<vis_types.h>");
	  addInclude("<vis_proto.h>");
	  addProcedure(quadmult,"CGCVISStereoBiquad_mult2x2d");
          addDeclaration(mainDecl);
	  addCode(settapDef);
	  addCode("$starSymbol(repeatstate1) = vis_fzeros();");
	  addCode("$starSymbol(repeatstate2) = vis_fzeros();");
	}
	codeblock(localDecl){
	  vis_f32 inhi,inlo,topbranch,bottbranch;
	  vis_f32 repeatnextstate,nextstate,statetmp,outtmp,outhi,outlo;
	}
        codeblock(iirfilter){
	  vis_write_gsr(($val(scalefactor)+1)<<3);

	  inhi = vis_read_hi($ref(input));
	  inlo = vis_read_lo($ref(input));

	  /* filter first stereo pair*/
	  topbranch =
	    $sharedSymbol(CGCVISStereoBiquad,mult2x2d)($starSymbol(repeatstate1),$starSymbol(repeatd1));
	  bottbranch =
	    $sharedSymbol(CGCVISStereoBiquad,mult2x2d)($starSymbol(repeatstate2),$starSymbol(repeatd2));

	  statetmp = vis_fpsub16s(inhi,topbranch);
	  repeatnextstate = vis_fpsub16s(statetmp,bottbranch);

	  statetmp =
	    $sharedSymbol(CGCVISStereoBiquad,mult2x2d)(repeatnextstate,$starSymbol(repeatn0));
	  topbranch = 
	    $sharedSymbol(CGCVISStereoBiquad,mult2x2d)($starSymbol(repeatstate1),$starSymbol(repeatn1));
	  bottbranch = 
	    $sharedSymbol(CGCVISStereoBiquad,mult2x2d)($starSymbol(repeatstate2),$starSymbol(repeatn2));

	  outtmp = vis_fpadd16s(statetmp,topbranch);
	  outhi = vis_fpadd16s(outtmp,bottbranch);

	  $starSymbol(repeatstate2)=$starSymbol(repeatstate1);
	  $starSymbol(repeatstate1)=repeatnextstate;

	  /* filter second stereo pair*/
	  topbranch =
	    $sharedSymbol(CGCVISStereoBiquad,mult2x2d)($starSymbol(repeatstate1),$starSymbol(repeatd1));
	  bottbranch =
	    $sharedSymbol(CGCVISStereoBiquad,mult2x2d)($starSymbol(repeatstate2),$starSymbol(repeatd2));

	  statetmp = vis_fpsub16s(inlo,topbranch);
	  repeatnextstate = vis_fpsub16s(statetmp,bottbranch);

	  statetmp =
	    $sharedSymbol(CGCVISStereoBiquad,mult2x2d)(repeatnextstate,$starSymbol(repeatn0));
	  topbranch = 
	    $sharedSymbol(CGCVISStereoBiquad,mult2x2d)($starSymbol(repeatstate1),$starSymbol(repeatn1));
	  bottbranch = 
	    $sharedSymbol(CGCVISStereoBiquad,mult2x2d)($starSymbol(repeatstate2),$starSymbol(repeatn2));

	  outtmp = vis_fpadd16s(statetmp,topbranch);
	  outlo = vis_fpadd16s(outtmp,bottbranch);

	  $starSymbol(repeatstate2)=$starSymbol(repeatstate1);
	  $starSymbol(repeatstate1)=repeatnextstate;

	  $ref(output) = vis_freg_pair(outhi,outlo);
	}
	go {
	  addCode(localDecl);
	  addCode(iirfilter);
	}
}
