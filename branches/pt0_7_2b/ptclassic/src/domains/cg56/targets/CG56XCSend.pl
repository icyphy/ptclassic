defstar	{

name { XCSend }
domain { CG56 }
desc { S56X to CGC sunchronous send star }
version	{ @(#)CG56XCSend.pl	1.16	01 Oct 1996 }
author { Jose Luis Pino }
derivedFrom { XCSynchComm }

copyright {
Copyright (c) 1993-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright	for copyright notice,
limitation of liability, and disclaimer	of warranty provisions.
}

ccinclude { "CGTarget.h" }
	
location { CG56	Target Directory }

explanation {}

protected {
	friend class S56XTargetWH;
}

input {
	name {input}
	type {ANYTYPE}
}

codeblock(sendData,"int pairNumber,int numXfer") {
$label(bufferFull)
	btst	#@(pairNumber%24),$ref(bufferSemaphore,@(pairNumber/24))
	jcs	$label(bufferFull)
	move	#$addr(input),r0	;read starting input geodesic address
	move	#$addr(buffer),r1	;read starting buffer address
	do	#@numXfer,$label(XFR)
		move	$mem(input):(r0)+,a
		move	a,$mem(buffer):(r1)+
$label(XFR)
}

setup {
    resolvedType = input.setResolvedType();
    numXfer = input.numXfer();
    CG56XCSynchComm::setup();
}
	
go {
	addCode(sendData(pairNumber,numXfer));
	addCode(processPendingInterrupts(pairNumber));
@	bset	#@(pairNumber%24),$ref(bufferSemaphore,@(pairNumber/24))
@
	addCode(triggerInterrupt);
}

execTime {
	if (input.numXfer()==1)	return 9;
	return (9 + 2*input.numXfer());
}

}
