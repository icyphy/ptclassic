defstar	{
name { XCPoke }
domain { CG56 }
desc { S56X to CGC synchronous send star }
version	{ $Id$ }
author { Jose Luis Pino }
derivedFrom { XCAsynchComm }

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

codeblock(sendBuffer) {
	movec	#($val(blockSize)-1),m0
	move	$ref(bufStart),r0
	move	$ref(input),x0
	move	x0,$mem(buffer):(r0)+
	move	r0,$ref(bufStart)
	movec	m1,m0			;restore m0
}

setup {
	CG56XCAsynchComm::setup();
}

go {
	if (blockSize > 1)
		addCode(sendBuffer);
	else
		addCode(sendData);
}

execTime {
	return 2;
}

}
