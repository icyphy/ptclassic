defstar {
	name { XCSReceive }
	domain { CG56 }
	desc { CGC to S56X Receive star }
	version { $Id$ }
	author { Jose L. Pino }
	derivedFrom {XCSBase}
	copyright {
Copyright (c) 1994,1993 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 Target Directory }
	ccinclude { "CGCXSReceive.h" }
	explanation {
	}
protected {
	friend class S56XTargetWH;
}
	output {
		name {output}
		type {ANYTYPE}
	}

codeblock(receiveData,"int pairNumber,int numXfer") {
$label(bufferEmpty)
	btst	#@(pairNumber%24),$ref(bufferSemaphore,@(pairNumber/24))
	jcc		$label(bufferEmpty)
	move	#$addr(output),r0	;read starting input geodesic address
	move	#$addr(buffer),r1	;read starting buffer address
	do		#@numXfer,$label(XFR)
			move	$mem(buffer):(r1)+,x0
			move	x0,$mem(output):(r0)+
$label(XFR)
	bclr	#@(pairNumber%24),$ref(bufferSemaphore,@(pairNumber/24))
}

setup {
	numXfer = output.numXfer();     
	CG56XCSBase::setup();      
}

go {
/*	if (output.far()->parent()->isA("CG56Collect")) {
		if (addCode("",NULL,output.far()->parent()->name())) {
			((CGStar*)output.far()->parent())->go();
		}
	} */
	addCode(receiveData(pairNumber,numXfer));
	addCode(interruptHost(pairNumber));
}

	execTime {
		if (output.numXfer()==1) return 9;
		return (9 + 2*output.numXfer());
	}
}
