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
	type {float}
    }

codeblock(receiveData) {
	int value;
	if ((value = qckPeek(dsp,"$val(VariableName)")) == -1) { 
		fprintf(stderr, "$val(VariableName): Receive Data Failed\n", qckErrString);
		exit(1);
	}
	if (value & 0x00800000) value |= 0xff000000;
}

codeblock(receiveBlock) {
	int value;
	if ((value = qckGetY(dsp,$starSymbol(bufferAddr)+i)) == -1) { 
		fprintf(stderr, "$val(VariableName)_buf %d: Receive Data Failed\n",
			i, qckErrString);
		exit(1);
	}
	if (value & 0x00800000) value |= 0xff000000;
}

	
codeblock(bufferRead) {
{
	int i;
	for (i = 0; i < $val(blockSize) ; i++) {
		$ref(output,i) = $starSymbol(buffer)[i];
	}
}
}

setup {
	if (blockSize > 1) output.setSDFParams(blockSize,blockSize-1);
	if (strcmp(output.resolvedType(),INT)==0)
		txType = "int";
	else
		txType = "double";
	CGCXABase::setup();
}
		
go {
	if (blockSize > 1) {
		StringList txCode = receiveBlock;
		txCode << "\t$starSymbol(buffer)[i] = ";
		if (strcmp(output.resolvedType(),INT)==0)
			txCode << "value;\n";
		else
			txCode << "(double)value/(1<<23);\n";
		StringList code;
		code << updateLink(txBlock(txCode)) << bufferRead;
		addCode(code);
	}
	else {
		StringList code = receiveData;
		code << "\t$ref(output) = ";
		if (strcmp(output.resolvedType(),INT)==0) 
			code << "value;\n";
		else
			code << "(double)value/(1<<23);\n";
		addCode(updateLink(code));
	}
}

}
