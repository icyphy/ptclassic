defstar {
    name { S56XReceive }
    domain { CGC }
    desc { Receive data from S56X to CGC }
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
    status = read(dsp->fd,buffer,@(numXfer*3));
    if (status == 0) {
	fprintf(stderr,"receive: DSP wormhole received premature EOF");
	exit(1);
    }
    if (status == -1) {
	fprintf(stderr,"receive: DSP wormhole receive timeout");
	exit(1);
    }
    for (int i = 0; i < @numXfer; i++) {
	int value = (buffer[3*i]<<16)+ (buffer[3*i+1]<<8)+ buffer[3*i+2];
	if (value & 0x00800000) value |= 0xff000000;
        @command;
    }
    }
    
    initCode {
	addInclude("<sys/types.h>");
	addInclude("<sys/uio.h>");
	addInclude("\"s56dspUser.h\"");
    }
    
    go {
	const char* intReceive = "$ref(output,i) = value";
	const char* fixReceive = "$ref(output,i) = value/(1<<(24-1))";
	if (output.resolvedType()==INT) 
	    addCode(receiveData(output.numXfer(),intReceive));
	else
	    addCode(receiveData(output.numXfer(),fixReceive));
    }
}
