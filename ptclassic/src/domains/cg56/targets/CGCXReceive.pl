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
    status = read(dsp->fd,buffer,@(numXfer*3));
    if (status == 0) {
	fprintf(stderr,"DSP read ioctl premature EOF\n");
	exit(1);
    }
    if (status == -1) {
	fprintf(stderr,"DSP read ioctl timeout\n");
	exit(1);
    }
    for (i = 0; i < @numXfer; i++) {
	int value = (buffer[3*i]<<16)+ (buffer[3*i+1]<<8)+ buffer[3*i+2];
	if (value & 0x00800000) value |= 0xff000000;
        @command;
    }
    }
    
    
    go {
	const char* intReceive = "$ref(output,i) = value";
	const char* fixReceive = "$ref(output,i) = (double)value/(1<<23)";
	if (output.resolvedType()==INT) 
	    addCode(receiveData(output.numXfer(),intReceive));
	else
	    addCode(receiveData(output.numXfer(),fixReceive));
    }
}
