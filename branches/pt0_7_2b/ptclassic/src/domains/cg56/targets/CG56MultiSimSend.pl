defstar {
	name { FictionSend }
	domain { CG56 }
	desc {
Send star for Fiction target
	}
	version { $Id$ }
	author { S. Ha and K. Asawaree }
	copyright { 1992 The Regents of the University of California }
	location { local library }
	input {
		name { input }
		type { fix }
	}
	public {
		OutCG56Port output;
	}
	constructor {
	output.setPort("output",this,FIX);
	}
	hinclude { "StringList.h" }
	ccinclude { <string.h>, "ProcMemory.h" }
	method {
		name {setProperty}
		type {"void"}
		access {public}
		arglist { "(int num)" }
		code {
			input.setSDFParams(num,0);
			output.setSDFParams(num,0);
		}
	}
// temporary solution...
	method {
		name {expandRef}
		type {"StringList"}
		access {protected}
		arglist {"(const char* s)"}
		code {
		if (strcmp(s,"output")) return CG56Star :: expandRef(s);
		StringList ref;
		ref << output.baseAddr();
		//ref << output.memory()->name() << ':' << output.baseAddr();
		return ref;
		}
	}
// later, we take care of numTokens > 1.
        codeblock (main) {
	move	$ref(input),x0
	move	#$ref(output),r0
	move	#>1,a1
$label(w_spin)
	jset	#0,x:(r0),$label(w_spin)
	or	x0,a
	move	a1,x:(r0)
	}
	go {
		addCode(main);
	}
        execTime {
		return 8;
	}
}
