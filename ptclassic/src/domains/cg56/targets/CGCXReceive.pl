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
	/* wait for dsp buffer to be full */
	while ( ~s56xSemaphores[@(pairNumber/24)] & semaphoreMask );
	for(i = 0 ; i<@numXfer ; i ++) {
		int value;
 		if ((value = qckGetY(dsp,@bufferPtr+i)) == -1) { 
			fprintf(stderr, "Receive Data Failed, Pair @pairNumber:	%s\n", qckErrString);
			exit(1);
		}
		if (value & 0x00800000) value |= 0xff000000;
		@command;
	}
	s56xSemaphores[@(pairNumber/24)] &= ~semaphoreMask;
	if (qckPutY(dsp,@(semaphorePtr+pairNumber/24),s56xSemaphores[@(pairNumber/24)]) == -1) { 
		fprintf(stderr, "Semaphore update failed, Pair @pairNumber:	%s\n", qckErrString);
		exit(1);
	}
}

go {
	const char* intReceive = "$ref(output,i) = value";
	const char* fixReceive = "$ref(output,i) = (double)value/(1<<23)";
	if (strcmp(output.resolvedType(),INT)==0) 
	    addCode(receiveData(intReceive,output.numXfer()));
	else
	    addCode(receiveData(fixReceive,output.numXfer()));
}

}


