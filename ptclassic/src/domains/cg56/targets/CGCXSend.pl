defstar {
    name { S56XSend }
    domain { CGC }
    desc { Send data from CGC to S56X }
    derivedFrom { S56XBase }
    version { $Id$ }
    author { Jose L. Pino }
    copyright { 
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 Target Directory }
    explanation {
    }
    input {
	    name {input}
	    type {ANYTYPE}
    }

    codeblock(DSPStartRead,"const char* filePrefix") {
	/* get the DSP parameters */
        if (ioctl(dsp->fd,DspGetParams, &dspParams) == -1) {
                fprintf(stderr,"Read failed on S-56X parameters");
                exit(1);
        }
        dspParams.startRead = qckLodGetIntr(dsp->prog,"STARTR");
        if (dspParams.startRead == -1) {
                fprintf(stderr,"No STARTR label in @filePrefix.lod");
                exit(1);
        }
        /* set the DSP parameters */
	if (ioctl(dsp->fd,DspSetParams, &dspParams) == -1) {
		fprintf(stderr,"Write failed on S-56X parameters");
		exit(1);
	}
    }
    
    codeblock(sendData,"int numXfer,const char* command") {
    char buffer[@(numXfer*3)];
    int status;
    int i;
    for (i = 0; i < @numXfer; i++) {
	int value;
	char* cvalue;
	@command;
	if (value < 0xff800000) value = 0xff800000;
	if (value > 0x007FFFFF) value = 0x007FFFFF;
	cvalue = (char*)&value;
	buffer[3*i]   = cvalue[1];
	buffer[3*i+1] = cvalue[2];
	buffer[3*i+2] = cvalue[3];
    }
    status = write(dsp->fd,buffer, @(numXfer*3));
    if (status == 0) {
	fprintf(stderr,"DSP write ioctl premature EOF\n");
	exit(1);
    }
    if (status == -1) {
	fprintf(stderr,"DSP write ioctl timeout\n");
	exit(1);
    }
    }

    initCode {
	CGCS56XBase::initCode();
	addMainInit(DSPStartRead(S56XFilePrefix),"STARTR");
    }

    go {
	const char* intSend = "value = $ref(input,i)";
	const char* fixSend = "value = $ref(input,i)*(1<<23)";
	if (input.resolvedType()==INT)
	    addCode(sendData(input.numXfer(),intSend));
	else
	    addCode(sendData(input.numXfer(),fixSend));
    }
}

