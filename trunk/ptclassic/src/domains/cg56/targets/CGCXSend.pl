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
    /* blocking write */
    printf("Writing @numXfer tokens to the DSP\n");
    status = write(dsp->fd,buffer, @(numXfer*3));
    if (status == 0) {
	perror("DSP write ioctl premature EOF");
	exit(1);
    }
    if (status == -1) {
	perror("DSP write ioctl timeout");
	exit(1);
    }
    }

codeblock(STARTW,"const char* filePrefix"){
	dspParams.startWrite= qckLodGetIntr(dsp->prog,"STARTW");
        if (dspParams.startWrite == -1) {
                perror("No STARTW label in @filePrefix.lod");
                exit(1);
        }
}

initCode {
	CGCS56XBase::initCode();
	addCode(STARTW(S56XFilePrefix),"S56XRoutines","STARTW");
}

    go {
	CGCS56XBase::go();
	const char* intSend = "value = $ref(input,i)";
	const char* fixSend = "value = $ref(input,i)*(1<<23)";
	if (strcmp(input.resolvedType(),INT)==0)
	    addCode(sendData(input.numXfer(),intSend));
	else
	    addCode(sendData(input.numXfer(),fixSend));
    }
}

