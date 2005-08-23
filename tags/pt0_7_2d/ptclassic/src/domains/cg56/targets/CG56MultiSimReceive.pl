defstar {
	name { MultiSimReceive }
	domain { CG56 }
	desc {
Receive star for Fiction target
	}
	version { @(#)CG56MultiSimReceive.pl	1.3	3/9/96 }
	author { S. Ha and K. Asawaree }
	copyright { 
Copyright (c) 1992-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { local library }
	output {
		name { output }
		type { fix }
	}
	public {
	InCG56Port input;
	}
	constructor {
	input.setPort("input", this, FIX);
	}
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
        hinclude { "StringList.h" }
        ccinclude { <string.h>, "ProcMemory.h"  }
// temporary solution
        method {
                name {expandRef}
                type {"StringList"}
                access {protected}
                arglist {"(const char* s)"}
                code {
                if (strcmp(s,"input")) return CG56Star :: expandRef(s);
                StringList ref;
                ref <<  input.baseAddr();
               // ref << input.memory()->name() << ':' << input.baseAddr();
                return ref;
		}
        }
// later take care numTokens > 1
        codeblock (main) {
	move	#$ref(input),r0
	move	#$$fffffe,a1
$label(r_spin)
	jclr	#0,x:(r0),$label(r_spin)
	move 	x:(r0),x0
	and	x0,a
	move	a1,x:(r0)
	move	a1,$ref(output)
	}
	go {
		addCode(main);
	}
        execTime {
		return 10;
	}
}
