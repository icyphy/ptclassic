defstar {
    name { XReceive }
    domain { CGC }
    desc { Receive data synchronously from S56X to CGC }
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
    explanation {
    }
    output {
	name {output}
	type {ANYTYPE}
    }

codeblock(receiveData,"const char* command, int numXfer") {
    int i, semaphoreMask = 1<<@(pairNumber%24);
    int count = S56X_MAX_POLL;
    int value[@numXfer];
    /* wait for dsp buffer to be full */
    while ( ! $val(S56XFilePrefix)_hostSemaphores[@pairNumber] && --count);
    $val(S56XFilePrefix)_hostSemaphores[@pairNumber]=0;
    if (count == 0) EXIT_CGC("The S-56X board is failing to send data.  Is there another process still attached to the DSP?");
#if @(numXfer-1)
    if (qckGetBlkItem($val(S56XFilePrefix)_dsp,$starSymbol(s56xBuffer),value,@numXfer) == -1)
#else
    if ((value[0] = qckPeekItem($val(S56XFilePrefix)_dsp,$starSymbol(s56xBuffer))) == -1)
#endif
     {
	char buffer[128];
	sprintf(buffer, "Receive Data Failed, Pair @pairNumber: %s",
		qckErrString);
	EXIT_CGC(buffer);
    }
    for (i=0;i<@numXfer;i++) {
	fix fixValue;
	char* pValue;
	pValue = (char*) &value[@numXfer-1-i];
	pValue++;
	memset(fixValue,0,sizeof(fixValue));
	memcpy(fixValue,pValue,3);
	@command;
    }
    $val(S56XFilePrefix)_s56xSemaphores[@(pairNumber/24)] &= ~semaphoreMask;
    if (qckPutY($val(S56XFilePrefix)_dsp,$val(S56XFilePrefix)_semaphorePtr+@(pairNumber/24),$val(S56XFilePrefix)_s56xSemaphores[@(pairNumber/24)]) == -1) { 
	char buffer[128];
	sprintf(buffer, "Semaphore update failed, Pair @pairNumber: %s", 
		qckErrString);
	EXIT_CGC(buffer);
    }
}

setup {
    if (output.setResolvedType() == FIX) {
	Precision cg56Fix(24,1);
	output.setPrecision(cg56Fix);
    }
    numXfer = output.numXfer();
    CGCXSynchComm::setup();
}

go {
    const char* intReceive = "$ref(output,i) = FIX_Fix2Int(24,24,fixValue)";
    const char* fixReceive = "FIX_Assign($ref(output,i),24,1,fixValue)";
    const char* floatReceive = "$ref(output,i)=FIX_Fix2Double(24,1,fixValue)";
    if (output.resolvedType() == INT) 
	addCode(receiveData(intReceive,output.numXfer()));
    else if (output.resolvedType() == FIX) 
	addCode(receiveData(fixReceive,output.numXfer()));
    else if (output.resolvedType() == FLOAT) 
	addCode(receiveData(floatReceive,output.numXfer()));
    else
	Error::abortRun(*this,output.resolvedType()," not supported");
}

}


