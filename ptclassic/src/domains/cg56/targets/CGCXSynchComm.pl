defstar {
    name { XSBase }
    domain { CGC }
    derivedFrom { XBase }
    desc { Base star for Synchronous CGC <-> S56X IPC }
    version { $Id$ }
    author { Jose Luis Pino }
    hinclude { "CG56XCSBase.h" }
    copyright { 
Copyright (c) 1994, 1993 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 Target Directory }
    explanation {
This star is the base star for all synchronous interprocessor communication
between the Sparc and S-56X board.
    }

ccinclude { <stdio.h> }

public {
	CG56XCSBase* s56xSide;
	int pairNumber;
	int commCount;
	unsigned int bufferPtr;
	unsigned int semaphorePtr;
}

codeblock(s56xInterrupt,"int currentBuffer") {
static void	s56xSignal() {
	/* The memory location y:@currentBuffer contains the pairNumber 
	   of send/receive buffer that just changed state, we store
	   that it is ready to process in the global array
	   s56xSemaphores
	 */
	int buffer;
	if ((buffer = qckGetY(dsp,@currentBuffer)-1) < 0) {
		fprintf(stderr, "S56X Buffer Status Update Failed");
		exit(1);
	}
	if (qckPutY(dsp,@currentBuffer,0) == -1) { 
		fprintf(stderr, "S56X Interrupt Reset Failed:	%s\n", qckErrString);
		exit(1);
	}
	s56xSemaphores[buffer/24] = qckGetY(dsp,@semaphorePtr+buffer/24);
} /* end s56xSignal interrupt */
}

codeblock(s56xSemaphoresInit,"int size") {
{
	int i;
	for (i = 0 ; i < @size ; i++); {
		s56xSemaphores[i] = 0;	/* all buffers emtpy */
	}
}
}

initCode {
	StringList s56xSemaphores;
	s56xSemaphores << "int s56xSemaphores[" << commCount/24 + 1 << "];";
	addGlobal(s56xSemaphores,"s56xSemaphores");
	addMainInit(s56xSemaphoresInit(commCount/24 + 1),"s56xSemaphoresInit");
	addMainInit("	signal(SIGUSR1,s56xSignal);");
	CGCXBase::initCode();
	s56xSide->lookupEntry("buffer",bufferPtr);
	s56xSide->lookupEntry("bufferSemaphore",semaphorePtr);
	int currentBuffer;
	s56xSide->lookupEntry("currentBuffer",currentBuffer);
	addGlobal(s56xInterrupt(currentBuffer),"s56xInterrupt");
}

}



