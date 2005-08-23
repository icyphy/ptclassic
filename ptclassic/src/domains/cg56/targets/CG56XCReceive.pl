defstar {
	name { XCReceive }
	domain { CG56 }
	desc { CGC to S56X Receive star }
	version { @(#)CG56XCReceive.pl	1.16	01 Oct 1996 }
	author { Jose L. Pino }
	derivedFrom {XCSynchComm}
	copyright {
Copyright (c) 1993-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 Target Directory }
	ccinclude { "CGCXReceive.h" }
	htmldoc {
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
			move	$mem(buffer):(r1)+,a
			move	a,$mem(output):(r0)+
$label(XFR)
}

setup {
    resolvedType = output.setResolvedType();
    numXfer = output.numXfer();     
    CG56XCSynchComm::setup();      
}

go {
	addCode(receiveData(pairNumber,numXfer));
	addCode(processPendingInterrupts(pairNumber));
@	bclr	#@(pairNumber%24),$ref(bufferSemaphore,@(pairNumber/24))
@
	addCode(triggerInterrupt);
}

	execTime {
		if (output.numXfer()==1) return 9;
		return (9 + 2*output.numXfer());
	}
}
