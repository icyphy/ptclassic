defstar {
	name {StereoBiquad}
	domain {CGC}
	version { $Id$ }
	desc {
A two-pole, two-zero parametric digital IIR filter (a biquad).
	}
	author { William Chen}
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC VIS library }
	explanation {
A two-pole, two-zero IIR filter.
	}
	input {
	  name{input}
	  type{float}
	}
	output {
	  name{output}
	  type{float}
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

	codeblock(quadmult) {
vis_d64 $sharedSymbol(CGCVISStereoBiquad,mult2x2d)(vis_f32 mult1,vis_f32 mult2) { 
	vis_d64 prodhi, prodlo, product;
	vis_f32 pack;

	prodhi = vis_fmuld8sux16(mult1,mult2);
	prodlo = vis_fmuld8ulx16(mult1,mult2);

	product = vis_fpadd32(prodhi,prodlo);
	return pack = vis_fpackfix(product);
}
	}

        codeblock(mainDecl){
	  vis_s16 $starSymbol(filtertaps)[5];
	  vis_f32 $starSymbol(repeatd1),$starSymbol(repeatd2);
	  vis_f32 $starSymbol(repeatn0),$starSymbol(repeatn1),$starSymbol(repeatn2);
	  vis_f32 $starSymbol(repeatstate1),$starSymbol(repeatstate2);
	}
	codeblock(settapDef){
	  $starSymbol(filtertaps)[0]=$val(scale)*$val(d1);
	  $starSymbol(filtertaps)[1]=$val(scale)*$val(d2);
	  $starSymbol(filtertaps)[2]=$val(scale)*$val(n0);
	  $starSymbol(filtertaps)[3]=$val(scale)*$val(n1);
	  $starSymbol(filtertaps)[4]=$val(scale)*$val(n2);

	  $starSymbol(repeatd1) =
	    vis_to_float($starSymbol(filtertaps)[0]<<16|$starSymbol(filtertaps)[0]);
	  $starSymbol(repeatd2) =
	    vis_to_float($starSymbol(filtertaps)[1]<<16|$starSymbol(filtertaps)[1]);
	  $starSymbol(repeatn0) =
	    vis_to_float($starSymbol(filtertaps)[2]<<16|$starSymbol(filtertaps)[2]);
	  $starSymbol(repeatn1) =
	    vis_to_float($starSymbol(filtertaps)[3]<<16|$starSymbol(filtertaps)[3]);
	  $starSymbol(repeatn2) =
	    vis_to_float($starSymbol(filtertaps)[4]<<16|$starSymbol(filtertaps)[4]);
	}
        initCode{
          addDeclaration(mainDecl);
	  addCode(settapDef);
	  addCode("$starSymbol(repeatstate1) = vis_fzeros();");
	  addcode("$starSymbol(repeatstate2) = vis_fzeros();");
	}
	codeblock(localDecl){
	  vis_f32 inhi,inlo,topbranch,bottbranch;
	  vis_f32 nextstate,statetmp,outtmp,outhi,outlo;
	}
        codeblock(iirfilter){
	  vis_write_gsr(8);

	  inhi = vis_readhi($ref(input));
	  inlo = vis_readlo($ref(input));

	  /* filter first stereo pair*/
	  topbranch =
	    $sharedSymbol(CGCVISStereoBiquad,mult2x2d)($starSymbol(repeatstate1),$starSymbol(repeatd1));
	  bottbranch =
	    $sharedSymbol(CGCVISStereoBiquad,mult2x2d)($starSymbol(repeatstate2),$starSymbol(repeatd2));

	  statetmp = vis_fsub16s(inhi,topbranch);
	  repeatnextstate = vis_fsub16s(statetmp,bottbranch);

	  statetmp =
	    $sharedSymbol(CGCVISStereoBiquad,mult2x2d)($starSymbol(repeatnextstate),$starSymbol(repeatn0));
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

	  statetmp = vis_fsub16s(inlo,topbranch);
	  repeatnextstate = vis_fsub16s(statetmp,bottbranch);

	  statetmp =
	    $sharedSymbol(CGCVISStereoBiquad,mult2x2d)($starSymbol(repeatnextstate),$starSymbol(repeatn0));
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
