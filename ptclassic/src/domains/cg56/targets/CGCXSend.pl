defstar {
    name { XSSend }
    domain { CGC }
    desc { Send data synchronously from CGC to S56X }
    derivedFrom { XSBase }
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

codeblock(sendData,"const char* command,int numXfer") {
	int i,semaphoreMask = 1<<@(pairNumber%24);
	/* wait for dsp buffer to be empty */
	while (s56xSemaphores[@(pairNumber/24)]&semaphoreMask);
	for(i = 0 ; i<@numXfer ; i ++) {
		int value;
		@command;
		if (value < 0xff800000) value = 0xff800000;
		if (value > 0x007FFFFF) value = 0x007FFFFF;
		if (qckPutY(dsp,@bufferPtr+i,value) == -1) { 
			fprintf(stderr, "Send Data Failed, Pair @pairNumber:	%s\n", qckErrString);
			exit(1);
		}
	}
	s56xSemaphores[@(pairNumber/24)] |= semaphoreMask;
	if (qckPutY(dsp,@(semaphorePtr+pairNumber/24),s56xSemaphores[@(pairNumber/24)]) == -1) { 
		fprintf(stderr, "Semaphore update failed, Pair @pairNumber:	%s\n", qckErrString);
		exit(1);
	}
}

go {
	const char* intSend = "value = $ref(input,i)";
	const char* fixSend = "value = $ref(input,i)*(1<<23)";
	if (strcmp(input.resolvedType(),INT)==0)
	    addCode(sendData(intSend,input.numXfer()));
	else
	    addCode(sendData(fixSend,input.numXfer()));
}

}

