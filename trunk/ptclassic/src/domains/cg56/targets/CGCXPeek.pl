defstar {
    name { XPeek }
    domain { CGC }
    desc { Receive data asynchronously from S56X to CGC }
    derivedFrom { XAsynchComm }
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

codeblock(receiveBlock,"") {
	if ( $starSymbol(bufStart) == 0) {
		int value[$val(blockSize)+1];
		int i;
		int j;
		int offset;
		if ( qckGetBlkItem(dsp,$starSymbol(s56xBuffer),value,$val(blockSize)+1) == -1) { 
			fprintf(stderr,"$val(VariableName): Receive Data Failed\n", qckErrString);
			exit(1);
		}
		offset = value[$val(blockSize)] - $starSymbol(bufferAddr);
		j = 0;
		for(i=offset;i<$val(blockSize);i++) {
			if (value[i] & 0x00800000) value[i] |= 0xff000000;
			$starSymbol(buffer)[j++] = @(strcmp(output.resolvedType(),INT)?"(double)value[i]/(1<<23)":"value[i]");
		}
		for(i=0;i<offset;i++) {
			if (value[i] & 0x00800000) value[i] |= 0xff000000;
			$starSymbol(buffer)[j++] = @(strcmp(output.resolvedType(),INT)?"(double)value[i]/(1<<23)":"value[i]");
		}
	}
}

	
codeblock(bufferRead) {
	$ref(output) = $starSymbol(buffer)[$starSymbol(bufStart)++];
	if ($starSymbol(bufStart) >= $val(blockSize))
		$starSymbol(bufStart) = 0;
}

setup {
	if (strcmp(output.resolvedType(),INT)==0)
		txType = "int";
	else
		txType = "double";
	CGCXAsynchComm::setup();
}
		
go {
	if (blockSize > 1) {
		StringList code;
		code << (const char*)receiveBlock() << bufferRead;
		addCode(code);
	}
	else {
		StringList code = (const char*) receiveData;
		code << "\t$ref(output) = ";
		if (strcmp(output.resolvedType(),INT)==0) 
			code << "value;\n";
		else
			code << "(double)value/(1<<23);\n";
		addCode(updateLink(code));
	}
}

}
