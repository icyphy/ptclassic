defstar {
    name { XAReceive }
    domain { CGC }
    desc { Receive data asynchronously from S56X to CGC }
    derivedFrom { XABase }
    version { $Id$ }
    author { Jose Luis Pino }
    copyright { 
Copyright (c) 1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    location { CG56 Target Directory }
    output {
	name {output}
	type {ANYTYPE}
    }

codeblock(receiveData) {
	int value;
	if ((value = qckPeek(dsp,"$val(VariableName)")) == -1) { 
		fprintf(stderr, "$val(VariableName): Receive Data Failed\n", qckErrString);
		exit(1);
	}
}

go {
	StringList code = receiveData;
	const char* intReceive = "\t$ref(output) = value;\n";
	const char* fixReceive = "\t$ref(output) = (double)value/(1<<23);\n";
	if (strcmp(output.resolvedType(),INT)==0) 
	    code << intReceive;
	else
	    code << fixReceive;
	addCode(updateLink(code));
}

}
