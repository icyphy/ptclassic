defstar {
    name { XBase }
    domain { CGC }
    desc { Base star from  CGC <-> S56X IPC }
    version { $Id$ }
    author { Jose Luis Pino }
    copyright { 
Copyright (c) 1993-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 Target Directory }
    explanation {
    }

ccinclude { <stdio.h>,"compat.h" }

state {
    name {S56XFilePrefix}
    type {string}
}

public {
    friend class S56XTarget;
}

codeblock(downloadCode,"const char* s56path") {
    /* open the DSP */
    if (($val(S56XFilePrefix)_dsp = qckAttach("/dev/s56dsp", NULL, 0)) == NULL) {
	perror("Could not access the S-56X Card");
	EXIT_CGC(0);
    }

    /* boot the moniter */
    if (qckBoot($val(S56XFilePrefix)_dsp,"@s56path/lib/qckMon.lod")==-1) {
	perror(qckErrString);
	EXIT_CGC(0);
    }

    /* load the application */
    if (qckLoad($val(S56XFilePrefix)_dsp,"@(cgTarget()->destDirectory)/$val(S56XFilePrefix).lod") == -1) {
	perror(qckErrString);
	EXIT_CGC(0);
    }
}

codeblock(signalSOL2){
{
    int sig = SIGUSR1;
    if (ioctl($val(S56XFilePrefix)_dsp->fd,DspSetAsyncSig, &sig) == -1) {
	perror("Setting of interrupt process pointer to S56X driver failed");
	EXIT_CGC(0);
    }
}
}

codeblock(startDSP) {    
    if (qckJsr($val(S56XFilePrefix)_dsp,"START") == -1) {
	perror(qckErrString);
	EXIT_CGC(0);
    }
}

initCode {
	addInclude("<unistd.h>");
        addCompileOption("-I$S56DSP/include");
        addLinkOption("-L$S56DSP/lib -l$QCKMON");
	addInclude("<sys/types.h>");
	addInclude("<sys/uio.h>");
	addInclude("<signal.h>");
	addInclude("<s56dspUser.h>");
	addInclude("<qckMon.h>");
	addInclude("<stdio.h>");
	addGlobal("    QckMon* $val(S56XFilePrefix)_dsp;","dsp");
	// We do this here so that all the stars can do their initialization
	// before starting the DSP
       	char *s56path = getenv("S56DSP");
	int newmemory = FALSE;
	if (s56path == NULL) {
	    s56path = expandPathName("$PTOLEMY/vendors/s56dsp");
	    newmemory = TRUE;
	}
	addMainInit(downloadCode(s56path),"s56load");
	addMainInit(signalSOL2,"s56signal");
	if ( newmemory) delete [] s56path;
}

wrapup {
	addMainInit(startDSP,"s56start");
	addCode("qckDetach($val(S56XFilePrefix)_dsp);$val(S56XFilePrefix)_dsp=0;\n","mainClose","qckDetach");
}

}



