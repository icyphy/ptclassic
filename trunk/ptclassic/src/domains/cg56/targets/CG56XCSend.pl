defstar	{

name { S56XCGCSend }
domain { CG56 }
desc { S56X to CGC send	star }
version	{ $Id$ }
author { Jose Pino }
derivedFrom { S56XCGCBase }

copyright {
Copyright (c) 1993 The Regents of the University of	California.
All	rights reserved.
See	the	file ~ptolemy/copyright	for	copyright notice,
limitation of liability, and disclaimer	of warranty	provisions.
}

ccinclude {	"CGTarget.h" }
	
location { CG56	Target Directory }

explanation	{}

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
	bset	#@(pairNumber%24),$ref(bufferSemaphore,@(pairNumber/24))
}

setup {
	numXfer = input.numXfer();
	CG56S56XCGCBase::setup();
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
}

execTime {
	if (input.numXfer()==1)	return 9;
	return (9 + 2*input.numXfer());
}

}
