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
    type {ANYTYPE}
}

codeblock(receiveToken,"") {
    int value;
    fix fixValue;
    char* pValue = (char*) &value;
    pValue++;
    if ((value = qckPeek(dsp,"$val(VariableName)")) == -1) { 
	fprintf(stderr, "$val(VariableName): Receive Data Failed\n", qckErrString);
	exit(1);
    }
    memset(fixValue,0,sizeof(fixValue));
    memcpy(fixValue,pValue,3);
    @command;
}

codeblock(receiveBlock,"") {
    if ( $starSymbol(bufStart) == 0) {
	int value[$val(blockSize)+1];
	int i;
	int offset;
	if ( qckGetBlkItem(dsp,$starSymbol(s56xBuffer),value,$val(blockSize)+1) == -1) { 
	    fprintf(stderr,"$val(VariableName): Receive Data Failed\n", qckErrString);
	    exit(1);
	}
	offset = value[$val(blockSize)] - $starSymbol(bufferAddr);
	for(i=0;i<$val(blockSize);i++) {
	    fix fixValue;
	    char* pValue = (char*) &value[offset++%$val(blockSize)];
	    pValue++;
	    memset(fixValue,0,sizeof(fixValue));
	    memcpy(fixValue,pValue,3);
	    @command
	}
    }
}


codeblock(resetBufferStart) {
    if ($starSymbol(bufStart) >= $val(blockSize)) $starSymbol(bufStart) = 0;
}

private {
    StringList command;
}

setup {
    int farXfer = output.far()->numXfer();
    if (!(int)blockSize%farXfer) output.setSDFParams(farXfer,farXfer-1);

    txType = output.resolvedType();

    StringList outVar;
    if ((int)blockSize > 1)
	outVar << "$starSymbol(buffer)[i]";
    else
	outVar << "$ref(output)";

    command.initialize();
    if (txType == INT) {
	command << outVar << " = FIX_Fix2Int(24,24,fixValue);";
    }
    else if (txType == FIX) {
	command << "FIX_Assign(" << outVar << ",24,1,fixValue);"; 
	Precision cg56Fix(24,1);
	output.setPrecision(cg56Fix);
    }
    else if (txType == FLOAT) {
	command << outVar << " = FIX_Fix2Double(24,1,fixValue);";
    }
    else {
	Error::abortRun(*this,"Incompatible type, ",output.resolvedType());
	return;
    }
    CGCXAsynchComm::setup();
}

go {
    if (blockSize > 1) {
	StringList code;
	StringList readBufferCommand;
	StringList refOutput;

	if (output.numXfer() > 1) {
	    refOutput = "$ref(output,i)";
	    readBufferCommand << "{\n\tint i;\n\tfor(i = 0;i < "
			      << output.numXfer() << ";i++) {\n";
	}
	else {
	    refOutput = "$ref(output)";
	}

	if (txType==FIX)
	    readBufferCommand
		<< "\tFIX_Assign(" << refOutput
		<< ",24,1,$starSymbol(buffer)[$starSymbol(bufStart)++]);\n";
	else
	    readBufferCommand
		<< "\t" << refOutput
		<< " = $starSymbol(buffer)[$starSymbol(bufStart)++];\n";
	if (output.numXfer() > 1) readBufferCommand << "}\n";

	code << receiveBlock() << readBufferCommand << resetBufferStart;
	addCode(code);
    }
    else {
	addCode(receiveToken());
    }
}

}

