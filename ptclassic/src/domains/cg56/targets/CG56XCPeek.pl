defstar {
    name { XCPeek }
    domain { CG56 }
    desc { CGC to S56X Asynchronous Receive star }
    version { $Id$ }
    author { Jose Luis Pino }
    derivedFrom {XCAsynchComm}
    copyright {
	   Copyright (c) 1994,1993 The Regents of the University of California.
	   All rights reserved.
	   See the file $PTOLEMY/copyright for copyright notice,
	   limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 Target Directory }
    explanation {
    }

output {
    name {output}
    type {ANYTYPE}
}

setup {
    CG56XCAsynchComm::setup();
//  We do not support buffers for peek yet
//  int farXfer = output.far()->numXfer();
//    if (!(int)blockSize%farXfer) output.setSDFParams(farXfer,farXfer-1);
    if ((int)blockSize > 1)
	Error::abortRun(*this," does not support blockSizes > 1");
}

codeblock(receiveData) {
	move	$ref(buffer),x0
	move	x0,$ref(output)
}

go {
	addCode(receiveData);
}

execTime {
	return 2;
}

}
