defstar {
    name { XBase }
    domain { CGC }
    desc { Base star from  CGC <-> S56X IPC }
    version { $Id$ }
    author { Jose Luis Pino }
    hinclude { "CGCS56XTarget.h" }
    copyright { 
Copyright (c) 1994,1993 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
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
	attributes { A_NONSETTABLE|A_NONCONSTANT}
	desc { 	The file prefix of the s56x generated files.  Set by 
			CGCS56XTarget::create{Send,Receive}
	}
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
}

}



