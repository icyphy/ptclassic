defstar	{
name { XCASend }
domain { CG56 }
desc { S56X to CGC synchronous send star }
version	{ $Id$ }
author { Jose Luis Pino }
derivedFrom { XCABase }

copyright {
Copyright (c) 1994, 1993 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright	for copyright notice,
limitation of liability, and disclaimer	of warranty provisions.
}

ccinclude { "CGTarget.h" }
	
location { CG56	Target Directory }

explanation {}

input {
	name {input}
	type {ANYTYPE}
}

codeblock(sendData) {
	move $ref(input),x0
	move x0,$ref(buffer)
}

codeblock(sendBuffer,"") {
;	btst	#0,$ref(semaphore)
;	jcs	$label(bufferLocked)
	move	#0,a
	move	$ref(semaphore),x0
	cmp	x0,a
	jne	$label(bufferLocked)
	bset	#1,$ref(semaphore)
	move	#$addr(input),r0	;read starting input geodesic address
	move	#$addr(buffer),r1	;read starting buffer address
	do	#@blockSize,$label(XFR)
	move	$mem(input):(r0)+,x0
	move	x0,$mem(buffer):(r1)+
$label(XFR)
	nop
	bclr	#1,$ref(semaphore)
$label(bufferLocked) 
}

setup {
	CG56XCABase::setup();
	input.setSDFParams(blockSize,blockSize-1);
}

go {
	if (blockSize > 1)
		addCode(sendBuffer());
	else
		addCode(sendData);
}

execTime {
	return 2;
}

}
