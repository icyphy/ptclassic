defstar {
	name {Biquad}
	domain {CGC}
	version {@(#)CGCBiquad.pl	1.3 6/29/96}
	desc {
A two-pole, two-zero parametric digital IIR filter (a biquad).
	}
	author { J. T. Buck and William Chen}
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC dsp library }
	explanation {
This two-pole, two-zero IIR filter.
	}
	input {
	  name{input}
	  type{float}
	}
	output {
	  name{output}
	  type{float}
	}
        defstate {
                name {d1}
                type { float }
                default { "-1.1430" }
        }
        defstate {
                name {d2}
                type { float }
                default { "0.41280" }
        }
        defstate {
                name {n0}
                type { float }
                default { "0.067455" }
        }
        defstate {
                name {n1}
                type { float }
                default { "0.135" }
        }
        defstate {
                name {n2}
                type { float }
                default { "0.067455" }
        }
	defstate {
	  name {state1}
	  type { float }
	  default { "0.0" }
	  desc { internal state. }
	  attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
	  name {state2}
	  type { float }
	  default { "0.0" }
	  desc {internal state. }
	  attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
        codeblock(mainDecl){
	  double $starSymbol(filtertaps)[5];
	}
	codeblock(settapDef){
	  $starSymbol(filtertaps)[0]=$val(d1);
	  $starSymbol(filtertaps)[1]=$val(d2);
	  $starSymbol(filtertaps)[2]=$val(n0);
	  $starSymbol(filtertaps)[3]=$val(n1);
	  $starSymbol(filtertaps)[4]=$val(n2);
	}
        initCode{
	  addInclude("<math.h>");
          addDeclaration(mainDecl);
	  addCode(settapDef);
	}
	codeblock(localDecl){
	  double nextstate,out;
	}
        codeblock(iirfilter){
	  nextstate = $ref(input) - $starSymbol(filtertaps)[0] *
	    (double)$ref(state1) - $starSymbol(filtertaps)[1] *
	    (double)$ref(state2);
	  out = nextstate * $starSymbol(filtertaps)[2] +
	    (double)$ref(state1) * $starSymbol(filtertaps)[3] +
	    (double)$ref(state2) * $starSymbol(filtertaps)[4];
	  $ref(output)=out;
	  $ref(state2)=$ref(state1);
	  $ref(state1)=nextstate;
	}
	go {
	  addCode(localDecl);
	  addCode(iirfilter);
	}
}
