defstar {
    name { XSynchComm }
    domain { CGC }
    derivedFrom { XBase }
    desc { Base star for Synchronous CGC <-> S56X IPC }
    version { $Id$ }
    author { Jose Luis Pino }
    hinclude { "CG56XCSynchComm.h" }
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

ccinclude { <stdio.h>,"compat.h" }

public {
	CG56XCSynchComm* s56xSide;
	int pairNumber;
	int commCount;
	unsigned int bufferPtr;
	unsigned int semaphorePtr;
	int numXfer;
}

codeblock(s56xInterrupt,"int currentBuffer") {
#ifdef __cplusplus
static void s56xSignal(int sig) 
#else
static void s56xSignal(sig) 
int sig;
#endif
{
	/* The memory location y:@currentBuffer contains the pairNumber 
	   of send/receive buffer that just changed state, we store
	   that it is ready to process in the global array
	   s56xSemaphores
	 */
	int buffer;
	if (!dsp) return;
	if ((buffer = qckGetY(dsp,@currentBuffer)-1) < 0) {
	        char buffer[128];
		sprintf(buffer, 
			"S56X Buffer Status Update Failed: %s", 
			qckErrString);
		EXIT_CGC(buffer);
	}
	if (qckPutY(dsp,@currentBuffer,0) == -1) { 
	        char buffer[128];
		sprintf(buffer, "S56X Interrupt Reset Failed: %s", 
			qckErrString);
		EXIT_CGC(buffer);
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

codeblock(loadDSPSymbols,"const char* varName") {
    if (($starSymbol(s56xBuffer) = qckItem(dsp,"@varName")) == 0)
	EXIT_CGC("CGCXSynchComm address resolution failed");
}

initCode {
	StringList s56xSemaphores;
	s56xSemaphores << "int s56xSemaphores[" << commCount/24 + 1 << "];";
	addDeclaration("QckItem $starSymbol(s56xBuffer);\n");
	addGlobal(s56xSemaphores,"s56xSemaphores");
        addGlobal("#define S56X_MAX_POLL 10000000","S56X_MAX_POLL");
	addMainInit(s56xSemaphoresInit(commCount/24 + 1),"s56xSemaphoresInit");
#ifdef PTSOL2
	addMainInit("	sigset(SIGUSR1,s56xSignal);","s56x_sigset");
	addMainInit("	dsp->intr=sbusMemHostIntr;","s56x_dspintr");
#else
	addMainInit("   signal(SIGUSR1,s56xSignal);");
#endif
	CGCXBase::initCode();
	s56xSide->lookupEntry("buffer",bufferPtr);
	s56xSide->lookupEntry("bufferSemaphore",semaphorePtr);
	unsigned int currentBuffer;
	s56xSide->lookupEntry("currentBuffer",currentBuffer);
	addGlobal(s56xInterrupt(currentBuffer),"s56xInterrupt");
}
wrapup {
    CGCXBase::wrapup();
    addMainInit(loadDSPSymbols(s56xSide->bufferName()));
}

}



