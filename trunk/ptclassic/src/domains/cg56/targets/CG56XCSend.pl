defstar	{

name { XCSend }
domain { CG56 }
desc { S56X to CGC sunchronous send star }
version	{ $Id$ }
author { Jose Luis Pino }
derivedFrom { XCSynchComm }

copyright {
Copyright (c) 1994, 1993 The Regents of the University of California.
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
		move	$mem(input):(r0)+,x0
		move	x0,$mem(buffer):(r1)+
$label(XFR)
}

setup {
	numXfer = input.numXfer();
	CG56XCSynchComm::setup();
}
	
go {
/* This is commented out because CG56Spread stars are not implemented yet.
   Furthermore, this will not compile under cfront go is protected.... it
   should probably be run.
  if (input.far()->parent()->isA("CG56Spread")) {
		if (addCode("",NULL,input.far()->parent()->name())) {
			((CGStar*)input.far()->parent())->go();
		}
	} */
	addCode(sendData(pairNumber,numXfer));
	addCode(interruptHost(pairNumber));
@	bset	#@(pairNumber%24),$ref(bufferSemaphore,@(pairNumber/24))
@
}

execTime {
	if (input.numXfer()==1)	return 9;
	return (9 + 2*input.numXfer());
}

}
