defstar {
    name { S56XReceive }
    domain { CGC }
    desc { Receive data from S56X to CGC }
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
    ccinclude {"CG56S56XCGCReceive.h" }
    explanation {
    }
    output {
	name {output}
	type {ANYTYPE}
    }

    codeblock(receiveData,"int numXfer,const char* command") {
    char buffer[@(numXfer*3)];
    int status;
    int i;
    /* blocking read */
    status = read(dsp->fd,buffer,@(numXfer*3));
    if (status == 0) {
	perror("DSP read ioctl premature EOF");
	exit(1);
    }
    if (status == -1) {
	perror("DSP read ioctl timeout");
	exit(1);
    }
    for (i = 0; i < @numXfer; i++) {
	int value = (buffer[3*i]<<16)+ (buffer[3*i+1]<<8)+ buffer[3*i+2];
	if (value & 0x00800000) value |= 0xff000000;
        @command;
    }
    }

    setup {
        CGCS56XBase::setup();
	PortHole* input = s56xSide->portWithName("input");
	DataType inputType = input->setResolvedType();
	if (strcmp(inputType,FIX) == 0) {
		output.setPort("output",this,FLOAT,output.numXfer());
	}
    }

    go {
	CGCS56XBase::go();
	const char* intReceive = "$ref(output,i) = value";
	const char* fixReceive = "$ref(output,i) = (double)value/(1<<23)";
	if (strcmp(output.resolvedType(),INT)==0) 
	    addCode(receiveData(output.numXfer(),intReceive));
	else
	    addCode(receiveData(output.numXfer(),fixReceive));
    }
}
