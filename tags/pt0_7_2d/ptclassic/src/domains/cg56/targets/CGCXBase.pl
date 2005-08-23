defstar {
    name { XBase }
    domain { CGC }
    derivedFrom { Fix }
    desc { Base star from CGC <-> S56X IPC }
    version { @(#)CGCXBase.pl	1.26	10/01/96 }
    author { Jose Luis Pino }
    copyright { 
Copyright (c) 1993-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 Target Directory }
	htmldoc {
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
	char message[1024];
	sprintf(message, "%s: %s", "Could not access the S-56X Card",
		strerror(errno));
	EXIT_CGC(message);
    }

    /* boot the monitor */
    if (qckBoot($val(S56XFilePrefix)_dsp,"@s56path/lib/qckMon.lod")==-1) {
	char message[1024];
	sprintf(message, "%s: %s", qckErrString, strerror(errno));
	EXIT_CGC(message);
    }

    /* load the application */
    if (qckLoad($val(S56XFilePrefix)_dsp,"@(cgTarget()->destDirectory)/$val(S56XFilePrefix).lod") == -1) {
	char message[1024];
	sprintf(message, "%s: %s", qckErrString, strerror(errno));
	EXIT_CGC(message);
    }
}

codeblock(signalSOL2){
{
    int sig = SIGUSR1;
    if (ioctl($val(S56XFilePrefix)_dsp->fd,DspSetAsyncSig, &sig) == -1) {
	char message[1024];
	sprintf(message, "%s: %s",
		"Setting of interrupt process pointer to S56X driver failed",
		strerror(errno));
	EXIT_CGC(message);
    }
}
}

codeblock(startDSP) {    
    if (qckJsr($val(S56XFilePrefix)_dsp,"START") == -1) {
	char message[1024];
	sprintf(message, "%s: %s", qckErrString, strerror(errno));
	EXIT_CGC(message);
    }
}

initCode {
	CGCFix::initCode();
        addInclude("<errno.h>");
	addInclude("<unistd.h>");
        addCompileOption("-I$S56DSP/include");
        addLinkOption("-L$S56DSP/lib");
	addLinkOption("-l$QCKMON");
	addInclude("<sys/types.h>");
	addInclude("<sys/uio.h>");
	addInclude("<signal.h>");
	addInclude("<s56dspUser.h>");
	addInclude("<qckMon.h>");
	addInclude("<stdio.h>");
	addInclude("<string.h>");
	addInclude("<memory.h>");
	addGlobal("    QckMon* $val(S56XFilePrefix)_dsp;","dsp");
	// We do this here so that all the stars can do their initialization
	// before starting the DSP
       	char *s56path = getenv("S56DSP");
	int newmemory = FALSE;
	if (s56path == NULL) {
	    s56path = expandPathName("$PTOLEMY/vendors/s56dsp");
	    newmemory = TRUE;
	}
	addMainInit(downloadCode(s56path), "s56load");
	addMainInit(signalSOL2, "s56signal");
	if ( newmemory) delete [] s56path;
}

wrapup {
	CGCFix::wrapup();
	addMainInit(startDSP,"s56start");
	addCode("qckDetach($val(S56XFilePrefix)_dsp);$val(S56XFilePrefix)_dsp=0;\n","mainClose","qckDetach");
}

}
