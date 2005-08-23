defstar {
    name { XSynchComm }
    domain { CGC }
    derivedFrom { XBase }
    desc { Base star for Synchronous CGC <-> S56X IPC }
    version { @(#)CGCXSynchComm.pl	1.12 01 Oct 1996 }
    author { Jose Luis Pino }
    hinclude { "CG56XCSynchComm.h" }
    copyright { 
Copyright (c) 1993-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 Target Directory }
	htmldoc {
This star is the base star for all synchronous interprocessor communication
between the Sparc and S-56X board.
}

    ccinclude { <stdio.h>,"compat.h" }

    public {
	CG56XCSynchComm* s56xSide;
	int pairNumber;
	int *commCount;
	unsigned int bufferPtr;
	int numXfer;
	DataType resolvedType;
    }

    codeblock(s56xInterrupt) {

#ifdef __cplusplus
    static void $val(S56XFilePrefix)_s56xSignal(int) 
#else
    static void $val(S56XFilePrefix)_s56xSignal(sig) 
        int sig;
#endif
    {
	/* The memory location y:@currentBuffer contains the pairNumber 
	   of send/receive buffer that just changed state, we store
	   that it is ready to process in the global array
	   $val(S56XFilePrefix)_s56xSemaphores
	   */
	int buffer;
	if (!$val(S56XFilePrefix)_dsp) return;
	if ((buffer = qckGetY($val(S56XFilePrefix)_dsp,
			      $val(S56XFilePrefix)_currentBuffer)-1) < 0) {
	    char buffer[128];
	    sprintf(buffer,"S56X Buffer Status Update Failed: %s",
		    qckErrString);
	    EXIT_CGC(buffer);
	}
	$val(S56XFilePrefix)_s56xSemaphores[buffer/24] =
 	    qckGetY($val(S56XFilePrefix)_dsp,
                    $val(S56XFilePrefix)_semaphorePtr+buffer/24);
	$val(S56XFilePrefix)_hostSemaphores[buffer] =
            !$val(S56XFilePrefix)_hostSemaphores[buffer];
	if (qckPutY($val(S56XFilePrefix)_dsp,
                    $val(S56XFilePrefix)_currentBuffer,0) == -1) { 
	    char buffer[128];
	    sprintf(buffer, "S56X Interrupt Reset Failed: %s", 
		    qckErrString);
	    EXIT_CGC(buffer);
	}
    } /* end $val(S56XFilePrefix)_s56xSignal interrupt */
    }

    codeblock(s56xSemaphoresInit,"int size") {
    {
	int i;
	for (i = 0 ; i < @(size/24 + 1) ; i++)
	    /* all buffers empty */
	    $val(S56XFilePrefix)_s56xSemaphores[i] = 0;
	for (i = 0 ; i < @size ; i++)
	    $val(S56XFilePrefix)_hostSemaphores[i] = 0;
    }
    }

    codeblock(loadDSPSymbols,"const char* varName,const char* type") {
	if (($starSymbol(s56xBuffer)@type =
             qckItem($val(S56XFilePrefix)_dsp,"@varName@type")) == 0)
	    EXIT_CGC("CGCXSynchComm @varName@type address resolution failed");
    }

    codeblock(loadSemaphoreItem) {
	if (($val(S56XFilePrefix)_s56xSemaphore = qckItem($val(S56XFilePrefix)_dsp,"PTOLEMY_S56X_SEM")) 
	    == 0)
	    EXIT_CGC("CGCXSynchComm semaphore address resolution failed");
	$val(S56XFilePrefix)_semaphorePtr = 
	    QckAddr($val(S56XFilePrefix)_s56xSemaphore);
	if (($val(S56XFilePrefix)_s56xCurrentBuffer =
             qckItem($val(S56XFilePrefix)_dsp,"PTOLEMY_S56X_BUF")) == 0)
	    EXIT_CGC("CGCXSynchComm current buffer address resolution failed");
	$val(S56XFilePrefix)_currentBuffer = 
	    QckAddr($val(S56XFilePrefix)_s56xCurrentBuffer);
    }

    codeblock(sigsetUSR1) {
    { 
	struct sigaction pt_alarm_action;
	sigset(SIGUSR1,$val(S56XFilePrefix)_s56xSignal);
	sigaction(SIGUSR1, 0, &pt_alarm_action);
	pt_alarm_action.sa_flags |= SA_RESTART;
	sigaction(SIGUSR1, &pt_alarm_action, 0);
    }
    }

    codeblock (semaphoreDeclaration,"int count") {
	/* We have a pair of redundant semaphore arrays.  The first
	   version keeps the semaphore as bits in a word.  The second
	   version keeps each semaphore as a separate integer.  This is
	   done for polling efficiency on the workstation */
	int $val(S56XFilePrefix)_s56xSemaphores[@(count/24 + 1)];
	volatile int $val(S56XFilePrefix)_hostSemaphores[@count];
    }
    
    initCode {
	addMainInit(s56xSemaphoresInit(*commCount),"s56xSemaphoresInit");
	CGCXBase::initCode();
	addMainInit(sigsetUSR1,"s56x_sigset");
	addMainInit("	$val(S56XFilePrefix)_dsp->intr=sbusMemHostIntr;","s56x_dspintr");
        addMainInit(loadSemaphoreItem,"s56x_sem_item");

        if (resolvedType == COMPLEX) {
	    addDeclaration("QckItem $starSymbol(s56xBuffer)_REAL;\n");
	    addDeclaration("QckItem $starSymbol(s56xBuffer)_IMAG;\n");
            addMainInit(loadDSPSymbols(s56xSide->bufferName(),"_REAL"));
            addMainInit(loadDSPSymbols(s56xSide->bufferName(),"_IMAG"));
        }
        else {
	    addDeclaration("QckItem $starSymbol(s56xBuffer);\n");
            addMainInit(loadDSPSymbols(s56xSide->bufferName(),""));
        }

        addDeclaration("QckItem $val(S56XFilePrefix)_s56xSemaphore;\n"
		       "QckItem $val(S56XFilePrefix)_s56xCurrentBuffer;\n");
	addGlobal("unsigned int $val(S56XFilePrefix)_semaphorePtr;\n"
		  "unsigned int $val(S56XFilePrefix)_currentBuffer;\n",
		  "s56xSemaphorePtr");
	addGlobal(semaphoreDeclaration(*commCount),"s56xSemaphores");
        addGlobal("#define S56X_MAX_POLL 10000000","S56X_MAX_POLL");
	addGlobal(s56xInterrupt,"s56xInterrupt");
    }

}



