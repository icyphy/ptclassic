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

go {
	addCode(sendData);
}

execTime {
	return 2;
}

}
