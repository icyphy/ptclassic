defstar {
    name { XSend }
    domain { CGC }
    desc { Send data synchronously from CGC to S56X }
    derivedFrom { XSynchComm }
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

codeblock(intSend,"int numXfer") {
{
    fix fixValue;
    memcpy(fixValue,&$ref(input@(numXfer>1?",i":"")),sizeof(int));
    FIX_Assign(24,24,dspWord,sizeof(int)*8,sizeof(int)*8,fixValue);
}
}

codeblock(floatSend,"int numXfer") {
    FIX_DoubleAssign(24,1,dspWord,$ref(input@(numXfer>1?",i":"")));
}

codeblock(fixSend,"int numXfer") {
    FIX_Assign(24,1,dspWord,$ref(input@(numXfer>1?",i":"")));
}

codeblock(sendData,"const char* command,int numXfer") {
    int value[@numXfer];
    int i,semaphoreMask = 1<<@(pairNumber%24);
    int count = S56X_MAX_POLL;
    /* wait for dsp buffer to be empty */
    while (--count && (s56xSemaphores[@(pairNumber/24)]&semaphoreMask));
    if (count == 0) EXIT_CGC("The S-56X board is failing to receive data.  Is there another process still attached to the DSP?");
    for(i = 0 ; i<@numXfer ; i ++) {
	char* pValue;
	fix dspWord;
        pValue = (char*) &value[@numXfer-1-i];
@command    
        memcpy(++pValue,dspWord,3);
    }

    if (qckPutBlkItem(dsp,value,$starSymbol(s56xBuffer),@numXfer) == -1) { 
	char buffer[128];
	sprintf(buffer,
		"Send Data Failed, Pair @pairNumber: %s", qckErrString);
	EXIT_CGC(buffer);
    }
    s56xSemaphores[@(pairNumber/24)] |= semaphoreMask;
    if (qckPutY(dsp,@(semaphorePtr+pairNumber/24),s56xSemaphores[@(pairNumber/24)]) == -1) { 
	char buffer[128];
	sprintf(buffer, "Semaphore update failed, Pair @pairNumber: %s", qckErrString);
	EXIT_CGC(buffer);
    }
}

setup {
    if (input.setResolvedType() == FIX) {
	Precision cg56Fix(24,1);
	input.setPrecision(cg56Fix);
    }
    numXfer = input.numXfer();
    CGCXSynchComm::setup();
}

go {
    StringList command;   
    if (input.resolvedType() == INT)
	command << intSend(input.numXfer());
    else if (input.resolvedType() == FIX)
	command << fixSend(input.numXfer());
    else if (input.resolvedType() == FLOAT)
	command << floatSend(input.numXfer());
    else
	Error::abortRun(*this,input.resolvedType()," not supported");
    addCode(sendData(command,input.numXfer()));
}

}






