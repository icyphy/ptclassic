defstar {
    name { S56XBase }
    domain { CGC }
    desc { Base star from  CGC to S56X Send Receive}
    version { $Id$ }
    author { Jose L. Pino }
    ccinclude { "CGTarget.h" } 
    copyright { 
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 Target Directory }
    explanation {
    }
    state {
	name { S56XFilePrefix }
	type { string }
	default { "" }
	desc { The file prefix of the s56x generated files.  Set by 
	       CGCS56XTarget::create{Send,Receive}
	}
    }
    codeblock(downloadCode,"const char* filePrefix,const char* s56path") {
	/* open the DSP */
	if ((dsp = qckAttach("/dev/s56dsp", NULL, 0)) == NULL) {
		fprintf(stderr,"Could not access the S-56X Card");
		exit(1);
	}

	/* boot the moniter */
	if (qckBoot(dsp,"@s56path/lib/qckMon.lod","@filePrefix.lod")==-1) {
		fprintf(stderr,qckErrString);
		exit(1);
	}

	/* load the application */
	if (qckLoad(dsp,"@filePrefix.lod") == -1) {
		fprintf(stderr,qckErrString);
		exit(1);
	}

	/* get the DSP parameters */
	if (ioctl(dsp->fd,DspGetParams, &dspParams) == -1) {
		fprintf(stderr,"Read failed on S-56X parameters");
		exit(1);
	}

	dspParams.dmaPageSize = 65536;
	dspParams.writeMode = DspWordCnt | DspPack24;
	dspParams.readMode = dspParams.writeMode;
	dspParams.topFill = 0;
	dspParams.midFill = 0;
	dspParams.dmaTimeout = 1000;

	/* set the DSP parameters */
	if (ioctl(dsp->fd,DspSetParams, &dspParams) == -1) {
		fprintf(stderr,"Write failed on S-56X parameters");
		exit(1);
	}

	if (qckJsr(dsp,"START") == -1) {
		fprintf(stderr,qckErrString);
		exit(1);
	}
    }

    initCode {
	addInclude("<sys/types.h>");
	addInclude("<sys/uio.h>");
	addInclude("<s56dspUser.h>");
	addInclude("<qckMon.h>");
	addInclude("<stdio.h>");
	addDeclaration("    QckMon* dsp;","dsp");
	addDeclaration("    Params dspParams;","dspParams");
	const char *s56path = getenv("S56DSP");
	if (s56path == NULL)
		s56path = expandPathName("$PTOLEMY/vendors/s56dsp");
	addMainInit(downloadCode(S56XFilePrefix,s56path),"s56load");
    }
    
}

