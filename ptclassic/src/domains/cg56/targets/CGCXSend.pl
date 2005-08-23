defstar {
    name { XSend }
    domain { CGC }
    desc { Send data synchronously from CGC to S56X }
    derivedFrom { XSynchComm }
    version { @(#)CGCXSend.pl	1.18	01 Oct 1996 }
    author { Jose Luis Pino }

copyright { 
Copyright (c) 1993-1996 The Regents of the University of California.
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

codeblock(intSend,"int numXfer") {
{
    fix fixValue;
    memcpy(fixValue,&$ref(input@(numXfer>1?",i":"")),sizeof(int));
    FIX_Assign(24,24,dspWord,sizeof(int)*8,sizeof(int)*8,fixValue);
}
}

codeblock(floatSend,"int numXfer,const char* type") {
    FIX_DoubleAssign(24,1,dspWord,$ref(input@(numXfer>1?",i":""))@type);
}

codeblock(fixSend,"int numXfer") {
    FIX_Assign(24,1,dspWord,$ref(input@(numXfer>1?",i":"")));
}

codeblock(poll,"") {
    int i,semaphoreMask = 1<<@(pairNumber%24);
    int count = S56X_MAX_POLL;
    /* wait for dsp buffer to be empty */
    while ($val(S56XFilePrefix)_hostSemaphores[@pairNumber] && --count);
    $val(S56XFilePrefix)_hostSemaphores[@pairNumber]=1;
    if (count == 0) EXIT_CGC("The S-56X buffer# @pairNumber is still empty.  The S-56X board is failing to receive data.  Is there another process still attached to the DSP?");
}

codeblock(sendData,"const char* command,int numXfer, const char* type") {
{
    int value[@numXfer];
    for(i = 0 ; i<@numXfer ; i ++) {
	char* pValue;
	fix dspWord;
        pValue = (char*) &value[@numXfer-1-i];
@command
        memcpy(++pValue,dspWord,3);
    }
#if @(numXfer-1)
    if (qckPutBlkItem($val(S56XFilePrefix)_dsp,value,$starSymbol(s56xBuffer)@type,@numXfer) == -1)
#else
    if (qckPokeItem($val(S56XFilePrefix)_dsp,$starSymbol(s56xBuffer)@type,value[0]) == -1)
#endif
    {
	char buffer[128];
	sprintf(buffer,
		"Send Data Failed, Pair @pairNumber: %s", qckErrString);
	EXIT_CGC(buffer);
    }
}
}

codeblock(updateSemaphore,"") {
    $val(S56XFilePrefix)_s56xSemaphores[@(pairNumber/24)] |= semaphoreMask;
    if (qckPutY($val(S56XFilePrefix)_dsp,$val(S56XFilePrefix)_semaphorePtr+@(pairNumber/24),$val(S56XFilePrefix)_s56xSemaphores[@(pairNumber/24)]) == -1) { 
	char buffer[128];
	sprintf(buffer, "Semaphore update failed, Pair @pairNumber: %s", qckErrString);
	EXIT_CGC(buffer);
    }
}

setup {
    resolvedType = input.setResolvedType();
    if (resolvedType == FIX) {
	Precision cg56Fix(24,1);
	input.setPrecision(cg56Fix);
    }
    numXfer = input.numXfer();
    CGCXSynchComm::setup();
}

go {
    addCode(poll());
    StringList command;
    StringList realOrImag;
    realOrImag = " ";
    if (input.resolvedType() == INT)
	command << intSend(input.numXfer());
    else if (input.resolvedType() == FIX)
	command << fixSend(input.numXfer());
    else if (input.resolvedType() == FLOAT)
	command << floatSend(input.numXfer(),"");
    else if (input.resolvedType() == COMPLEX) {
	command << floatSend(input.numXfer(),".real");
	realOrImag = "_REAL";
	addCode(sendData(command,input.numXfer(),realOrImag));
	command = floatSend(input.numXfer(),".imag");
	realOrImag = "_IMAG";
    }
    else {
	Error::abortRun(*this,input.resolvedType()," not supported");
	return;
    }
    addCode(sendData(command,input.numXfer(),realOrImag));
    addCode(updateSemaphore());
}

}






