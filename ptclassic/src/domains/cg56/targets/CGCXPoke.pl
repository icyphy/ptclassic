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
    type {fix}
    default {0}
    attributes {A_NONCONSTANT|A_NONSETTABLE}
}

codeblock(intSend) {
    fix fixValue,dspWord;
    memcpy(fixValue,&$ref(input),sizeof(int));
    FIX_Assign(24,24,dspWord,sizeof(int)*8,sizeof(int)*8,fixValue);
}

codeblock(floatSend) {
    fix dspWord;
    FIX_DoubleAssign(24,1,dspWord,$ref(input));
}

codeblock(fixSend) {
    fix dspWord;
    FIX_Assign(24,1,dspWord,$ref(input));
}

codeblock(sendData,"") {
    if (FIX_Compare(24,1,dspWord,$ref(buffer))) {
	int value;
	char* pValue = (char*) &value;
	memcpy(++pValue,dspWord,3);
	if (qckPoke(dsp,"$val(VariableName)",value) == -1) { 
	    fprintf(stderr, "$val(VariableName):Send Data Failed\n", qckErrString);
	    exit(1);
	}
	FIX_Assign($ref(buffer),24,1,dspWord);
    }
}

setup {
    int farXfer = input.far()->numXfer();
    if (!(int)blockSize%farXfer) input.setSDFParams(farXfer,farXfer-1);
    if ((int) blockSize > 1) {
	Error::abortRun(*this," does not support blockSizes > 1");
	return;
    }
    txType = input.resolvedType();
    Precision cg56Fix(24,1);
    buffer.setPrecision(cg56Fix);
    if (input.resolvedType() == FIX)
	input.setPrecision(cg56Fix);
    CGCXAsynchComm::setup();
}

go {
    StringList code;
    if (txType == INT)
	code << intSend;
    else if (txType == FIX)
	code << fixSend;
    else if (txType == FLOAT)
	code << floatSend;
    else {
	Error::abortRun(*this,"Incompatible type, ",input.resolvedType());
	return;
    }
    code << sendData();
    addCode(updateLink(code));
}

}

