defstar {
    name { XBase }
    domain { CGC }
    desc { Base star from  CGC <-> S56X IPC }
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

ccinclude { <stdio.h>,"compat.h" }

state {
	name { S56XFilePrefix }
	type { string }
	default { "" }
	attributes { A_NONSETTABLE|A_NONCONSTANT}
	desc { 	The file prefix of the s56x generated files.  Set by 
			CGCS56XTarget::create{Send,Receive}
	}
}

codeblock(downloadCode,"const char* filePrefix,const char* s56path") {
    /* open the DSP */
    if ((dsp = qckAttach("/dev/s56dsp", NULL, 0)) == NULL) {
	perror("Could not access the S-56X Card");
	EXIT_CGC(0);
    }

    /* boot the moniter */
    if (qckBoot(dsp,"@s56path/lib/qckMon.lod")==-1) {
	perror(qckErrString);
	EXIT_CGC(0);
    }

    /* load the application */
    if (qckLoad(dsp,"@filePrefix.lod") == -1) {
	perror(qckErrString);
	EXIT_CGC(0);
    }
}

codeblock(signalSOL2){
{
    int sig = SIGUSR1;
    if (ioctl(dsp->fd,DspSetAsyncSig, &sig) == -1) {
	perror("Setting of interrupt process pointer to S56X driver failed");
	EXIT_CGC(0);
    }
}
}

codeblock(signalSUN4) {
{
    Params dspParams;
    /* get the DSP parameters */
    if (ioctl(dsp->fd,DspGetParams, (char*) &dspParams) == -1) {
	perror("Read failed on S-56X parameters");
	EXIT_CGC(0);
    }

    dspParams.writeMode = DspWordCnt | DspPack24;
    dspParams.readMode = dspParams.writeMode;
    dspParams.topFill = 0;
    dspParams.midFill = 0;
    dspParams.dmaTimeout = 1000;

    dspParams.signal = SIGUSR1;

    /* set the DSP parameters */
    if (ioctl(dsp->fd,DspSetParams, (char*) &dspParams) == -1) {
	perror("Write failed on S-56X parameters");
	EXIT_CGC(0);
    }
}
}

codeblock(startDSP) {    
    if (qckJsr(dsp,"START") == -1) {
	perror(qckErrString);
	EXIT_CGC(0);
    }
}

initCode {
	addInclude("<sys/types.h>");
	addInclude("<sys/uio.h>");
	addInclude("<signal.h>");
	addInclude("<s56dspUser.h>");
	addInclude("<qckMon.h>");
	addInclude("<stdio.h>");
	addGlobal("    QckMon* dsp;","dsp");
}

wrapup {
	// We do this here so that all the stars can do there initialization
	// before starting the DSP
       	const char *s56path = getenv("S56DSP");
	if (s56path == NULL)
		s56path = expandPathName("$PTOLEMY/vendors/s56dsp");
	addMainInit(downloadCode(S56XFilePrefix,s56path),"s56load");
#ifdef PTSOL2
	addMainInit(signalSOL2,"s56signal");
#else
	addMainInit(signalSUN4,"s56signal");
#endif
	addMainInit(startDSP,"s56start");
	addCode("qckDetach(dsp);dsp=0;\n","mainClose","qckDetach");
}

}



