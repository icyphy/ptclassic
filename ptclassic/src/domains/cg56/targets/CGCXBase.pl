defstar {
    name { S56XBase }
    domain { CGC }
    desc { Base star from  CGC to S56X Send Receive}
    version { $Id$ }
    author { Jose L. Pino }
    hinclude { "CGCS56XTarget.h" "CG56S56XCGCBase.h" }
    copyright { 
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 Target Directory }
    explanation {
    }

ccinclude { <stdio.h> }

state {
	name { S56XFilePrefix }
	type { string }
	default { "" }
	desc { 	The file prefix of the s56x generated files.  Set by 
			CGCS56XTarget::create{Send,Receive}
	}
}

protected {
	friend class CGCS56XTarget;
	CG56S56XCGCBase* s56xSide;
	int pairNumber;
	int commCount;
	unsigned int bufferPtr;
	unsigned int semaphorePtr;
}

codeblock(downloadCode,"const char* filePrefix,const char* s56path") {
{
	Params dspParams;
	/* open the DSP */
	if ((dsp = qckAttach("/dev/s56dsp", NULL, 0)) == NULL) {
		perror("Could not access the S-56X Card");
		exit(1);
	}

	/* boot the moniter */
	if (qckBoot(dsp,"@s56path/lib/qckMon.lod","@filePrefix.lod")==-1) {
		perror(qckErrString);
		exit(1);
	}

	/* load the application */
	if (qckLoad(dsp,"@filePrefix.lod") == -1) {
		perror(qckErrString);
		exit(1);
	}

	/* get the DSP parameters */
	if (ioctl(dsp->fd,DspGetParams, &dspParams) == -1) {
		perror("Read failed on S-56X parameters");
		exit(1);
	}

	dspParams.writeMode = DspWordCnt | DspPack24;
	dspParams.readMode = dspParams.writeMode;
	dspParams.topFill = 0;
	dspParams.midFill = 0;
	dspParams.dmaTimeout = 1000;

	dspParams.signal = SIGUSR1;
	signal(SIGUSR1,s56xSignal);

	/* set the DSP parameters */
	if (ioctl(dsp->fd,DspSetParams, &dspParams) == -1) {
		perror("Write failed on S-56X parameters");
		exit(1);
	}
	if (qckJsr(dsp,"START") == -1) {
		perror(qckErrString);
		exit(1);
	}
}
}

codeblock(s56xInterrupt,"int currentBuffer,int semaphorePtr") {
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
	s56xSide->lookupEntry("buffer",bufferPtr);
	s56xSide->lookupEntry("bufferSemaphore",semaphorePtr);
	int currentBuffer;
	s56xSide->lookupEntry("currentBuffer",currentBuffer);
	addInclude("<sys/types.h>");
	addInclude("<sys/uio.h>");
	addInclude("<signal.h>");
	addInclude("<s56dspUser.h>");
	addInclude("<qckMon.h>");
	addInclude("<stdio.h>");
	addGlobal("    QckMon* dsp;","dsp");
	StringList s56xSemaphores;
	s56xSemaphores << "	int s56xSemaphores[" << commCount/24 + 1 << "];";
	addGlobal(s56xSemaphores,"s56xSemaphores");
	addGlobal(s56xInterrupt(currentBuffer,semaphorePtr),"s56xInterrupt");
	addMainInit(s56xSemaphoresInit(commCount/24 + 1),"s56xSemaphoresInit");
	const char *s56path = getenv("S56DSP");
	if (s56path == NULL)
		s56path = expandPathName("$PTOLEMY/vendors/s56dsp");
	addMainInit(downloadCode(S56XFilePrefix,s56path),"s56load");
}

}



