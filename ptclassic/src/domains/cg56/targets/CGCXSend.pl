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
    for (int i = 0; i < @numXfer; i++) {
	int value;
	@command;
	if (value < int(0xff800000)) value = 0xff800000;
	if (value > int(0x007FFFFF)) value = 0x007FFFFF;
	char* cvalue = (char*)&value;
	buffer[3*i]   = cvalue[1];
	buffer[3*i+1] = cvalue[2];
	buffer[3*i+2] = cvalue[3];
    }
    status = write(dsp->fd,buffer, size*3);
    if (status == 0) {
	fprintf(stderr,"send: DSP wormhole received premature EOF");
	exit(1);
    }
    if (status == -1) {
	fprintf(stderr,"send: DSP wormhole receive timeout");
	exit(1);
    }
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

