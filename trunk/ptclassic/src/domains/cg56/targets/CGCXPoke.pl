defstar {
    name { XPoke }
    domain { CGC }
    desc { Send data asynchronously from CGC to S56X }
    derivedFrom { XAsynchComm }
    version { $Id$ }
    author { Jose Luis Pino }

copyright { 
Copyright (c) 1994,1993 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}

location { CG56 Target Directory }

explanation {}

input {
	name {input}
	type {ANYTYPE}
}

state {
	name {buffer}
	type {int}
	default {0}
	attributes {A_NONCONSTANT|A_NONSETTABLE}
}

codeblock(sendData) {
	if (value != $ref(buffer)) {
		$ref(buffer) = value;
		if (qckPoke(dsp,"$val(VariableName)",value) == -1) { 
			fprintf(stderr, "$val(VariableName):Send Data Failed\n", qckErrString);
			exit(1);
		}
        }
}

go {
	StringList code = "\tint value;\n";
	const char* intSend = "\tvalue = $ref(input);\n";
	const char* fixSend = "\tvalue = $ref(input)*(1<<23);\n";
	if (strcmp(input.resolvedType(),INT)==0)
	    code << intSend;
	else
	    code << fixSend;
	code << sendData;
	addCode(updateLink(code));
}

}

